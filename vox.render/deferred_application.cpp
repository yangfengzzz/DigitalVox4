//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "deferred_application.h"
#include "rendering/subpasses/shadow_subpass.h"
#include "rendering/subpasses/deferred_subpass.h"
#include "rendering/subpasses/compose_subpass.h"
#include "rendering/subpasses/point_light_subpass.h"
#include "rendering/subpasses/skybox_subpass.h"
#include "rendering/subpasses/particle_subpass.h"
#include "engine.h"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

namespace vox {
using namespace simd;

DeferredApplication::~DeferredApplication() {
    delete m_completedHandler;
}

bool DeferredApplication::prepare(Engine &engine) {
    MetalApplication::prepare(engine);
        
    _scene = std::make_unique<Scene>();
    
    auto extent = engine.window().extent();
    auto scale = engine.window().contentScaleFactor();
    loadScene();
    
    m_inFlightSemaphore = dispatch_semaphore_create(MaxFramesInFlight);
#pragma mark Shadow render pass descriptor setup
    {
        MTL::TextureDescriptor shadowTextureDesc;
        shadowTextureDesc.pixelFormat(MTL::PixelFormatDepth16Unorm);
        shadowTextureDesc.width(2048);
        shadowTextureDesc.height(2048);
        shadowTextureDesc.mipmapLevelCount(1);
        shadowTextureDesc.resourceOptions(MTL::ResourceStorageModePrivate);
        shadowTextureDesc.usage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
        
        m_shadowMap = _device->makeTexture(shadowTextureDesc);
        m_shadowMap.label("Shadow Map");
        m_shadowRenderPassDescriptor.depthAttachment.texture( m_shadowMap );
        m_shadowRenderPassDescriptor.depthAttachment.loadAction(MTL::LoadActionClear);
        m_shadowRenderPassDescriptor.depthAttachment.storeAction(MTL::StoreActionStore);
        m_shadowRenderPassDescriptor.depthAttachment.clearDepth(1.0);
        m_shadowRenderPass = std::make_unique<RenderPass>(_library, &m_shadowRenderPassDescriptor);
        m_shadowRenderPass->addSubpass(std::make_unique<ShadowSubpass>(_renderView.get(), _scene.get(), nullptr));
    }
    
#pragma mark GBuffer render pass descriptor setup
    {
        m_albedo_specular_GBufferFormat = MTL::PixelFormatRGBA8Unorm_sRGB;
        m_normal_shadow_GBufferFormat = MTL::PixelFormatRGBA8Snorm;
        m_depth_GBufferFormat = MTL::PixelFormatR32Float;
        MTL::TextureDescriptor GBufferTextureDesc;
        
        GBufferTextureDesc.pixelFormat(MTL::PixelFormatRGBA8Unorm_sRGB);
        GBufferTextureDesc.width(extent.width * scale);
        GBufferTextureDesc.height(extent.height * scale);
        GBufferTextureDesc.mipmapLevelCount(1);
        GBufferTextureDesc.textureType(MTL::TextureType2D);
        GBufferTextureDesc.usage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
        GBufferTextureDesc.storageMode(MTL::StorageModePrivate);
        GBufferTextureDesc.pixelFormat(m_albedo_specular_GBufferFormat);
        m_albedo_specular_GBuffer = _device->makeTexture(GBufferTextureDesc);
        m_albedo_specular_GBuffer.label("Albedo + Shadow GBuffer");
        GBufferTextureDesc.pixelFormat(m_normal_shadow_GBufferFormat);
        m_normal_shadow_GBuffer = _device->makeTexture(GBufferTextureDesc);
        m_normal_shadow_GBuffer.label("Normal + Specular GBuffer");
        GBufferTextureDesc.pixelFormat(m_depth_GBufferFormat);
        m_depth_GBuffer = _device->makeTexture(GBufferTextureDesc);
        m_depth_GBuffer.label("Depth GBuffer");
        
        // Create a render pass descriptor to create an encoder for rendering to the GBuffers.
        // The encoder stores rendered data of each attachment when encoding ends.
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetLighting].loadAction(MTL::LoadActionDontCare);
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetLighting].storeAction(MTL::StoreActionDontCare);
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].loadAction(MTL::LoadActionDontCare);
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].storeAction(MTL::StoreActionStore);
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].texture(m_albedo_specular_GBuffer);
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetNormal].loadAction(MTL::LoadActionDontCare);
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetNormal].storeAction(MTL::StoreActionStore);
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetNormal].texture(m_normal_shadow_GBuffer);
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetDepth].loadAction(MTL::LoadActionDontCare);
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetDepth].storeAction(MTL::StoreActionStore);
        m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetDepth].texture(m_depth_GBuffer);
        m_GBufferRenderPassDescriptor.depthAttachment.clearDepth(1.0);
        m_GBufferRenderPassDescriptor.depthAttachment.loadAction(MTL::LoadActionClear);
        m_GBufferRenderPassDescriptor.depthAttachment.storeAction(MTL::StoreActionStore);
        m_GBufferRenderPassDescriptor.depthAttachment.texture(*_renderView->depthStencilTexture());
        m_GBufferRenderPassDescriptor.stencilAttachment.clearStencil(0);
        m_GBufferRenderPassDescriptor.stencilAttachment.loadAction(MTL::LoadActionClear);
        m_GBufferRenderPassDescriptor.stencilAttachment.storeAction(MTL::StoreActionStore);
        m_GBufferRenderPassDescriptor.stencilAttachment.texture(*_renderView->depthStencilTexture());
        m_GBufferRenderPass = std::make_unique<RenderPass>(_library, &m_GBufferRenderPassDescriptor);
        m_GBufferRenderPass->addParentPass(DeferredSubpass::dependedPassName(), m_shadowRenderPass.get());
        m_GBufferRenderPass->addSubpass(std::make_unique<DeferredSubpass>(_renderView.get(), _scene.get(), nullptr));
    }
    
#pragma mark Compositor render pass descriptor setup
    {
        // Create a render pass descriptor for thelighting and composition pass
        // Whatever rendered in the final pass needs to be stored so it can be displayed
        m_finalRenderPassDescriptor.colorAttachments[0].storeAction(MTL::StoreActionStore);
        m_finalRenderPassDescriptor.depthAttachment.loadAction(MTL::LoadActionLoad);
        m_finalRenderPassDescriptor.depthAttachment.texture(*_renderView->depthStencilTexture());
        m_finalRenderPassDescriptor.stencilAttachment.loadAction(MTL::LoadActionLoad);
        m_finalRenderPassDescriptor.stencilAttachment.texture(*_renderView->depthStencilTexture());
        m_finalRenderPass = std::make_unique<RenderPass>(_library, &m_finalRenderPassDescriptor);
        m_finalRenderPass->addParentPass(ComposeSubpass::dependedPassName(), m_GBufferRenderPass.get());
        m_finalRenderPass->addSubpass(std::make_unique<ComposeSubpass>(_renderView.get(), _scene.get(), nullptr));
//        m_finalRenderPass->addSubpass(std::make_unique<PointLightSubpass>(_renderView.get(), _scene.get(), nullptr,
//                                                                          m_icosahedronMesh, NumLights));
        m_finalRenderPass->addSubpass(std::make_unique<SkyboxSubpass>(_renderView.get(), _scene.get(), nullptr));
//        m_finalRenderPass->addSubpass(std::make_unique<ParticleSubpass>(_renderView.get(), _scene.get(), nullptr,
//                                                                        m_fairy, m_fairyMap, NumLights, NumFairyVertices));
    }
    framebufferResize(extent.width * scale, extent.height * scale);
    
    return true;
}

void DeferredApplication::update(float delta_time) {
    MetalApplication::update(delta_time);
    
    // Wait to ensure only MaxFramesInFlight are getting processed by any stage in the Metal
    // pipeline (App, Metal, Drivers, GPU, etc)
    dispatch_semaphore_wait(m_inFlightSemaphore, DISPATCH_TIME_FOREVER);
    
    {
        // Create a new command buffer for each render pass to the current drawable
        MTL::CommandBuffer commandBuffer = _commandQueue.commandBuffer();
        commandBuffer.label("Shadow & GBuffer Commands");
        
        m_shadowRenderPass->draw(commandBuffer);
        m_GBufferRenderPass->draw(commandBuffer, "GBuffer Generation");
        
        // Commit commands so that Metal can begin working on non-drawable dependant work without
        // waiting for a drawable to become avaliable
        commandBuffer.commit();
    }
    
    {
        MTL::CommandBuffer commandBuffer = _commandQueue.commandBuffer();
        if (!m_completedHandler) {
            // Create a completed handler functor for Metal to execute when the GPU has fully finished
            // processing the commands encoded for this frame.  This implenentation of the completed
            // hander signals the `m_inFlightSemaphore`, which indicates that the GPU is no longer
            // accesing the the dynamic buffer written this frame.  When the GPU no longer accesses the
            // buffer, the Subpass can safely overwrite the buffer's data to update data for a future
            // frame.
            struct CommandBufferCompletedHandler : public MTL::CommandBufferHandler {
                dispatch_semaphore_t semaphore;
                
                void operator()(const MTL::CommandBuffer &) {
                    dispatch_semaphore_signal(semaphore);
                }
            };
            
            CommandBufferCompletedHandler *completedHandler = new CommandBufferCompletedHandler();
            completedHandler->semaphore = m_inFlightSemaphore;
            
            m_completedHandler = completedHandler;
        }
        commandBuffer.addCompletedHandler(*m_completedHandler);
        commandBuffer.label("Lighting Commands");
        
        MTL::Drawable *drawable = _renderView->currentDrawable();
        // The final pass can only render if a drawable is available, otherwise it needs to skip
        // rendering this frame.
        if (drawable) {
            // Render the lighting and composition pass
            m_finalRenderPassDescriptor.colorAttachments[0].texture(*drawable->texture());
            m_finalRenderPassDescriptor.depthAttachment.texture(*_renderView->depthStencilTexture());
            m_finalRenderPassDescriptor.stencilAttachment.texture(*_renderView->depthStencilTexture());
            m_finalRenderPass->draw(commandBuffer, "Lighting & Composition Pass");
        }
        
        // Schedule a present once the framebuffer is complete using the current drawable
        if (_renderView->currentDrawable()) {
            // Create a scheduled handler functor for Metal to present the drawable when the command
            // buffer has been scheduled by the kernel.
            struct PresentationScheduledHandler : public MTL::CommandBufferHandler {
                MTL::Drawable m_drawable;
                
                PresentationScheduledHandler(MTL::Drawable drawable)
                : m_drawable(drawable) {
                }
                
                void operator()(const MTL::CommandBuffer &) {
                    m_drawable.present();
                    delete this;
                }
            };
            
            PresentationScheduledHandler *scheduledHandler = new PresentationScheduledHandler(*_renderView->currentDrawable());
            commandBuffer.addScheduledHandler(*scheduledHandler);
        }
        
        // Finalize rendering here & push the command buffer to the GPU
        commandBuffer.commit();
    }
}

void DeferredApplication::framebufferResize(uint32_t width, uint32_t height) {    
    MetalApplication::framebufferResize(width, height);
    m_GBufferRenderPassDescriptor.depthAttachment.texture(*_renderView->depthStencilTexture());
    m_GBufferRenderPassDescriptor.stencilAttachment.texture(*_renderView->depthStencilTexture());
    
    MTL::TextureDescriptor GBufferTextureDesc;
    GBufferTextureDesc.pixelFormat(MTL::PixelFormatRGBA8Unorm_sRGB);
    GBufferTextureDesc.width(width);
    GBufferTextureDesc.height(height);
    GBufferTextureDesc.mipmapLevelCount(1);
    GBufferTextureDesc.textureType(MTL::TextureType2D);
    GBufferTextureDesc.usage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
    GBufferTextureDesc.storageMode(MTL::StorageModePrivate);
    GBufferTextureDesc.pixelFormat(m_albedo_specular_GBufferFormat);
    m_albedo_specular_GBuffer = _device->makeTexture(GBufferTextureDesc);
    m_albedo_specular_GBuffer.label("Albedo + Shadow GBuffer");
    GBufferTextureDesc.pixelFormat(m_normal_shadow_GBufferFormat);
    m_normal_shadow_GBuffer = _device->makeTexture(GBufferTextureDesc);
    m_normal_shadow_GBuffer.label("Normal + Specular GBuffer");
    GBufferTextureDesc.pixelFormat(m_depth_GBufferFormat);
    m_depth_GBuffer = _device->makeTexture(GBufferTextureDesc);
    m_depth_GBuffer.label("Depth GBuffer");
    // Re-set GBuffer textures in the GBuffer render pass descriptor after they have been
    // reallocated by a resize
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetAlbedo].texture(m_albedo_specular_GBuffer);
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetNormal].texture(m_normal_shadow_GBuffer);
    m_GBufferRenderPassDescriptor.colorAttachments[RenderTargetDepth].texture(m_depth_GBuffer);
}



}
