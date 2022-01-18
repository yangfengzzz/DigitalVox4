//
//  forward.cpp
//  apps
//
//  Created by 杨丰 on 2022/1/16.
//

#include "forward.h"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"
#include "rendering/render_pass.h"
#include "rendering/subpasses/forward_subpass.h"

#include "engine.h"
#include "core/CPPMetalAssert.hpp"
#include "material/texture_loader.h"
#include "graphics/mesh_loader.h"
#include "graphics/primitive_mesh.h"
#include "shader/shader.h"
#include "camera.h"

namespace vox {
using namespace simd;

Forward::~Forward() {
    render_pipeline.reset();
}

bool Forward::prepare(Engine &engine) {
    MetalApplication::prepare(engine);
    MTL::Library shaderLibrary = makeShaderLibrary();
    auto extent = engine.get_window().get_extent();
    render_context->resize(MTL::SizeMake(extent.width * 2, extent.height * 2, 0));
    render_context->depthStencilPixelFormat(MTL::PixelFormatDepth32Float_Stencil8);
    render_context->colorPixelFormat(MTL::PixelFormatBGRA8Unorm_sRGB);
    
    {
        // Positions.
        m_defaultVertexDescriptor.attributes[VertexAttributePosition].format(MTL::VertexFormatFloat3);
        m_defaultVertexDescriptor.attributes[VertexAttributePosition].offset(0);
        m_defaultVertexDescriptor.attributes[VertexAttributePosition].bufferIndex(BufferIndexMeshPositions);
        
        // Texture coordinates.
        m_defaultVertexDescriptor.attributes[VertexAttributeTexcoord].format(MTL::VertexFormatFloat2);
        m_defaultVertexDescriptor.attributes[VertexAttributeTexcoord].offset(0);
        m_defaultVertexDescriptor.attributes[VertexAttributeTexcoord].bufferIndex(BufferIndexMeshGenerics);
        
        // Normals.
        m_defaultVertexDescriptor.attributes[VertexAttributeNormal].format(MTL::VertexFormatHalf4);
        m_defaultVertexDescriptor.attributes[VertexAttributeNormal].offset(8);
        m_defaultVertexDescriptor.attributes[VertexAttributeNormal].bufferIndex(BufferIndexMeshGenerics);
        
        // Tangents
        m_defaultVertexDescriptor.attributes[VertexAttributeTangent].format(MTL::VertexFormatHalf4);
        m_defaultVertexDescriptor.attributes[VertexAttributeTangent].offset(16);
        m_defaultVertexDescriptor.attributes[VertexAttributeTangent].bufferIndex(BufferIndexMeshGenerics);
        
        // Bitangents
        m_defaultVertexDescriptor.attributes[VertexAttributeBitangent].format(MTL::VertexFormatHalf4);
        m_defaultVertexDescriptor.attributes[VertexAttributeBitangent].offset(24);
        m_defaultVertexDescriptor.attributes[VertexAttributeBitangent].bufferIndex(BufferIndexMeshGenerics);
        
        // Position Buffer Layout
        m_defaultVertexDescriptor.layouts[BufferIndexMeshPositions].stride(12);
        m_defaultVertexDescriptor.layouts[BufferIndexMeshPositions].stepRate(1);
        m_defaultVertexDescriptor.layouts[BufferIndexMeshPositions].stepFunction(MTL::VertexStepFunctionPerVertex);
        
        // Generic Attribute Buffer Layout
        m_defaultVertexDescriptor.layouts[BufferIndexMeshGenerics].stride(32);
        m_defaultVertexDescriptor.layouts[BufferIndexMeshGenerics].stepRate(1);
        m_defaultVertexDescriptor.layouts[BufferIndexMeshGenerics].stepFunction(MTL::VertexStepFunctionPerVertex);
    }
    loadScene();
    
#pragma mark Compositor render pass descriptor setup
    {
        // Create a render pass descriptor for thelighting and composition pass
        // Whatever rendered in the final pass needs to be stored so it can be displayed
        m_finalRenderPassDescriptor.colorAttachments[0].storeAction(MTL::StoreActionStore);
        m_finalRenderPassDescriptor.colorAttachments[0].loadAction(MTL::LoadActionClear);
        m_finalRenderPassDescriptor.depthAttachment.loadAction(MTL::LoadActionClear);
        m_finalRenderPassDescriptor.depthAttachment.texture(*render_context->depthStencilTexture());
        m_finalRenderPassDescriptor.stencilAttachment.loadAction(MTL::LoadActionClear);
        m_finalRenderPassDescriptor.stencilAttachment.texture(*render_context->depthStencilTexture());
        m_finalRenderPass = std::make_unique<RenderPass>(&m_finalRenderPassDescriptor);
        m_finalRenderPass->addSubpass(std::make_unique<ForwardSubpass>(&m_finalRenderPassDescriptor, scene.get(), m_camera,
                                                                       shaderLibrary, *device, MTL::PixelFormatBGRA8Unorm_sRGB));
    }
    framebuffer_resize(extent.width*2, extent.height*2);
    
    return true;
}

void Forward::loadScene() {
    auto rootEntity = scene->createRootEntity();
    auto modelEntity = rootEntity->createChild();
    newMeshesFromBundlePath("../assets/Models", "Temple.obj",
                            *device, modelEntity, m_defaultVertexDescriptor);
    modelEntity->transform->setPosition(0, 10, 0);
    modelEntity->transform->setScale(0.1, 0.1, 0.1);
    
    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(-6.02535057, 36.6681671, 48.6991844);
    m_camera = cameraEntity->addComponent<Camera>();
    controller = cameraEntity->addComponent<control::OrbitControl>();
}

void Forward::input_event(const InputEvent &input_event) {
    controller->inputEvent(input_event);
}

void Forward::update(float delta_time) {
    MetalApplication::update(delta_time);
    
    MTL::CommandBuffer commandBuffer = m_commandQueue.commandBuffer();
    MTL::Drawable *drawable = render_context->currentDrawable();
    // The final pass can only render if a drawable is available, otherwise it needs to skip
    // rendering this frame.
    if (drawable) {
        // Render the lighting and composition pass
        m_finalRenderPassDescriptor.colorAttachments[0].texture(*drawable->texture());
        m_finalRenderPassDescriptor.depthAttachment.texture(*render_context->depthStencilTexture());
        m_finalRenderPassDescriptor.stencilAttachment.texture(*render_context->depthStencilTexture());
        m_finalRenderPass->draw(commandBuffer, "Lighting & Composition Pass");
    }
    // Finalize rendering here & push the command buffer to the GPU
    commandBuffer.commit();
    drawable->present();
}

void Forward::framebuffer_resize(uint32_t width, uint32_t height) {
    // When reshape is called, update the aspect ratio and projection matrix since the view
    //   orientation or size has changed
    m_camera->resize(width, height);
    
    MetalApplication::framebuffer_resize(width, height);
}

}
