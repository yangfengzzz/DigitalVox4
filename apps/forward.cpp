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

    loadScene();
    auto extent = engine.window().extent();
    _camera->resize(extent.width*2, extent.height*2);
    _renderView->resize(MTL::sizeMake(extent.width * 2, extent.height * 2, 0));
    _renderView->depthStencilPixelFormat(MTL::PixelFormatDepth32Float_Stencil8);
    _renderView->colorPixelFormat(MTL::PixelFormatBGRA8Unorm_sRGB);
    
#pragma mark Compositor render pass descriptor setup
    {
        // Create a render pass descriptor for thelighting and composition pass
        // Whatever rendered in the final pass needs to be stored so it can be displayed
        _finalRenderPassDescriptor.colorAttachments[0].storeAction(MTL::StoreActionStore);
        _finalRenderPassDescriptor.colorAttachments[0].loadAction(MTL::LoadActionClear);
        _finalRenderPassDescriptor.depthAttachment.loadAction(MTL::LoadActionClear);
        _finalRenderPassDescriptor.depthAttachment.texture(*_renderView->depthStencilTexture());
        _finalRenderPassDescriptor.stencilAttachment.loadAction(MTL::LoadActionClear);
        _finalRenderPassDescriptor.stencilAttachment.texture(*_renderView->depthStencilTexture());
        _finalRenderPass = std::make_unique<RenderPass>(_library, &_finalRenderPassDescriptor);
        _finalRenderPass->addSubpass(std::make_unique<ForwardSubpass>(_renderView.get(), _scene.get(), _camera));
    }
    
    return true;
}

void Forward::loadScene() {
    auto rootEntity = _scene->createRootEntity();
    auto modelEntity = rootEntity->createChild();
    
    MeshLoader loader(_device.get());
    loader.loadMesh("../assets/Models", "Temple.obj", modelEntity);
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
    MTL::Drawable *drawable = _renderView->currentDrawable();
    // The final pass can only render if a drawable is available, otherwise it needs to skip
    // rendering this frame.
    if (drawable) {
        // Render the lighting and composition pass
        _finalRenderPassDescriptor.colorAttachments[0].texture(*drawable->texture());
        _finalRenderPassDescriptor.depthAttachment.texture(*_renderView->depthStencilTexture());
        _finalRenderPassDescriptor.stencilAttachment.texture(*_renderView->depthStencilTexture());
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
