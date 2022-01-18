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
#include "core/cpp_mtl_assert.h"
#include "material/texture_loader.h"
#include "graphics/mesh_loader.h"
#include "graphics/primitive_mesh.h"
#include "shader/shader.h"
#include "camera.h"

namespace vox {
using namespace simd;

Forward::~Forward() {
    _renderPipeline.reset();
}

bool Forward::prepare(Engine &engine) {
    MetalApplication::prepare(engine);
    MTL::Library shaderLibrary = makeShaderLibrary();
    auto extent = engine.window().extent();
    _renderContext->resize(MTL::sizeMake(extent.width * 2, extent.height * 2, 0));
    _renderContext->depthStencilPixelFormat(MTL::PixelFormatDepth32Float_Stencil8);
    _renderContext->colorPixelFormat(MTL::PixelFormatBGRA8Unorm_sRGB);
    
    {
        // Positions.
        _defaultVertexDescriptor.attributes[VertexAttributePosition].format(MTL::VertexFormatFloat3);
        _defaultVertexDescriptor.attributes[VertexAttributePosition].offset(0);
        _defaultVertexDescriptor.attributes[VertexAttributePosition].bufferIndex(BufferIndexMeshPositions);
        
        // Texture coordinates.
        _defaultVertexDescriptor.attributes[VertexAttributeTexcoord].format(MTL::VertexFormatFloat2);
        _defaultVertexDescriptor.attributes[VertexAttributeTexcoord].offset(0);
        _defaultVertexDescriptor.attributes[VertexAttributeTexcoord].bufferIndex(BufferIndexMeshGenerics);
        
        // Normals.
        _defaultVertexDescriptor.attributes[VertexAttributeNormal].format(MTL::VertexFormatHalf4);
        _defaultVertexDescriptor.attributes[VertexAttributeNormal].offset(8);
        _defaultVertexDescriptor.attributes[VertexAttributeNormal].bufferIndex(BufferIndexMeshGenerics);
        
        // Tangents
        _defaultVertexDescriptor.attributes[VertexAttributeTangent].format(MTL::VertexFormatHalf4);
        _defaultVertexDescriptor.attributes[VertexAttributeTangent].offset(16);
        _defaultVertexDescriptor.attributes[VertexAttributeTangent].bufferIndex(BufferIndexMeshGenerics);
        
        // Bitangents
        _defaultVertexDescriptor.attributes[VertexAttributeBitangent].format(MTL::VertexFormatHalf4);
        _defaultVertexDescriptor.attributes[VertexAttributeBitangent].offset(24);
        _defaultVertexDescriptor.attributes[VertexAttributeBitangent].bufferIndex(BufferIndexMeshGenerics);
        
        // Position Buffer Layout
        _defaultVertexDescriptor.layouts[BufferIndexMeshPositions].stride(12);
        _defaultVertexDescriptor.layouts[BufferIndexMeshPositions].stepRate(1);
        _defaultVertexDescriptor.layouts[BufferIndexMeshPositions].stepFunction(MTL::VertexStepFunctionPerVertex);
        
        // Generic Attribute Buffer Layout
        _defaultVertexDescriptor.layouts[BufferIndexMeshGenerics].stride(32);
        _defaultVertexDescriptor.layouts[BufferIndexMeshGenerics].stepRate(1);
        _defaultVertexDescriptor.layouts[BufferIndexMeshGenerics].stepFunction(MTL::VertexStepFunctionPerVertex);
    }
    loadScene();
    
#pragma mark Compositor render pass descriptor setup
    {
        // Create a render pass descriptor for thelighting and composition pass
        // Whatever rendered in the final pass needs to be stored so it can be displayed
        _finalRenderPassDescriptor.colorAttachments[0].storeAction(MTL::StoreActionStore);
        _finalRenderPassDescriptor.colorAttachments[0].loadAction(MTL::LoadActionClear);
        _finalRenderPassDescriptor.depthAttachment.loadAction(MTL::LoadActionClear);
        _finalRenderPassDescriptor.depthAttachment.texture(*_renderContext->depthStencilTexture());
        _finalRenderPassDescriptor.stencilAttachment.loadAction(MTL::LoadActionClear);
        _finalRenderPassDescriptor.stencilAttachment.texture(*_renderContext->depthStencilTexture());
        _finalRenderPass = std::make_unique<RenderPass>(&_finalRenderPassDescriptor);
        _finalRenderPass->addSubpass(std::make_unique<ForwardSubpass>(&_finalRenderPassDescriptor, _device.get(), _scene.get(), _camera,
                                                                      shaderLibrary, MTL::PixelFormatBGRA8Unorm_sRGB));
    }
    framebufferResize(extent.width*2, extent.height*2);
    
    return true;
}

void Forward::loadScene() {
    auto rootEntity = _scene->createRootEntity();
    auto modelEntity = rootEntity->createChild();
    newMeshesFromBundlePath("../assets/Models", "Temple.obj",
                            *_device, modelEntity, _defaultVertexDescriptor);
    modelEntity->transform->setPosition(0, 10, 0);
    modelEntity->transform->setScale(0.1, 0.1, 0.1);
    
    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(-6.02535057, 36.6681671, 48.6991844);
    _camera = cameraEntity->addComponent<Camera>();
    _controller = cameraEntity->addComponent<control::OrbitControl>();
}

void Forward::inputEvent(const InputEvent &inputEvent) {
    _controller->inputEvent(inputEvent);
}

void Forward::update(float delta_time) {
    MetalApplication::update(delta_time);
    
    MTL::CommandBuffer commandBuffer = _commandQueue.commandBuffer();
    MTL::Drawable *drawable = _renderContext->currentDrawable();
    // The final pass can only render if a drawable is available, otherwise it needs to skip
    // rendering this frame.
    if (drawable) {
        // Render the lighting and composition pass
        _finalRenderPassDescriptor.colorAttachments[0].texture(*drawable->texture());
        _finalRenderPassDescriptor.depthAttachment.texture(*_renderContext->depthStencilTexture());
        _finalRenderPassDescriptor.stencilAttachment.texture(*_renderContext->depthStencilTexture());
        _finalRenderPass->draw(commandBuffer, "Lighting & Composition Pass");
    }
    // Finalize rendering here & push the command buffer to the GPU
    commandBuffer.commit();
    drawable->present();
}

void Forward::framebufferResize(uint32_t width, uint32_t height) {
    // When reshape is called, update the aspect ratio and projection matrix since the view
    //   orientation or size has changed
    _camera->resize(width, height);
    
    MetalApplication::framebufferResize(width, height);
}

}
