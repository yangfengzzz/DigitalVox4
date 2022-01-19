//
//  forward.cpp
//  apps
//
//  Created by 杨丰 on 2022/1/16.
//

#include "forward_application.h"
#include "rendering/subpasses/forward_subpass.h"
#include "engine.h"
#include "camera.h"

namespace vox {
ForwardApplication::~ForwardApplication() {
    _renderPipeline.reset();
}

bool ForwardApplication::prepare(Engine &engine) {
    MetalApplication::prepare(engine);
    
    auto extent = engine.window().extent();
    auto scale = engine.window().contentScaleFactor();
    loadScene(extent.width * scale, extent.height * scale);
    
    // Create a render pass descriptor for thelighting and composition pass
    // Whatever rendered in the final pass needs to be stored so it can be displayed
    _finalRenderPassDescriptor.colorAttachments[0].storeAction(MTL::StoreActionStore);
    _finalRenderPassDescriptor.colorAttachments[0].loadAction(MTL::LoadActionClear);
    _finalRenderPassDescriptor.depthAttachment.loadAction(MTL::LoadActionClear);
    _finalRenderPassDescriptor.depthAttachment.texture(*_renderView->depthStencilTexture());
    _finalRenderPassDescriptor.stencilAttachment.loadAction(MTL::LoadActionClear);
    _finalRenderPassDescriptor.stencilAttachment.texture(*_renderView->depthStencilTexture());
    _finalRenderPass = std::make_unique<RenderPass>(_library, &_finalRenderPassDescriptor);
    _finalRenderPass->addSubpass(std::make_unique<ForwardSubpass>(_renderView.get(), _scene.get(), _mainCamera));
    
    return true;
}

void ForwardApplication::update(float delta_time) {
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

void ForwardApplication::framebufferResize(uint32_t width, uint32_t height) {
    MetalApplication::framebufferResize(width, height);

    _mainCamera->resize(width, height);
}

}
