//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "forward_application.h"
#include "rendering/subpasses/forward_subpass.h"
#include "engine.h"
#include "camera.h"

namespace vox {
ForwardApplication::~ForwardApplication() {
    _renderPass.reset();
}

bool ForwardApplication::prepare(Engine &engine) {
    MetalApplication::prepare(engine);
    
    _scene = std::make_unique<Scene>();
    
    auto extent = engine.window().extent();
    loadScene(extent.width, extent.height);
    
    // Create a render pass descriptor for thelighting and composition pass
    // Whatever rendered in the final pass needs to be stored so it can be displayed
    _renderPassDescriptor.colorAttachments[0].storeAction(MTL::StoreActionStore);
    _renderPassDescriptor.colorAttachments[0].loadAction(MTL::LoadActionClear);
    _renderPassDescriptor.depthAttachment.loadAction(MTL::LoadActionClear);
    _renderPassDescriptor.depthAttachment.texture(*_renderView->depthStencilTexture());
    _renderPassDescriptor.stencilAttachment.loadAction(MTL::LoadActionClear);
    _renderPassDescriptor.stencilAttachment.texture(*_renderView->depthStencilTexture());
    _renderPass = std::make_unique<RenderPass>(_library, &_renderPassDescriptor);
    _renderPass->addSubpass(std::make_unique<ForwardSubpass>(_renderView.get(), _scene.get(), _mainCamera));
    if (_gui) {
        _renderPass->setGUI(_gui.get());
    }
    
    return true;
}

void ForwardApplication::update(float delta_time) {
    MetalApplication::update(delta_time);
    _scene->update(delta_time);
    _scene->updateShaderData(_device.get());

    MTL::CommandBuffer commandBuffer = _commandQueue.commandBuffer();
    MTL::Drawable *drawable = _renderView->currentDrawable();
    // The final pass can only render if a drawable is available, otherwise it needs to skip
    // rendering this frame.
    if (drawable) {
        // Render the lighting and composition pass
        _renderPassDescriptor.colorAttachments[0].texture(*drawable->texture());
        _renderPassDescriptor.depthAttachment.texture(*_renderView->depthStencilTexture());
        _renderPassDescriptor.stencilAttachment.texture(*_renderView->depthStencilTexture());
        _renderPass->draw(commandBuffer, "Lighting & Composition Pass");
    }
    // Finalize rendering here & push the command buffer to the GPU
    commandBuffer.commit();
    drawable->present();
}

bool ForwardApplication::resize(uint32_t win_width, uint32_t win_height,
                                uint32_t fb_width, uint32_t fb_height) {
    MetalApplication::resize(win_width, win_height, fb_width, fb_height);
    _scene->updateSize(win_width, win_height, fb_width, fb_height);
    _mainCamera->resize(win_width, win_height);
    return true;
}

void ForwardApplication::inputEvent(const InputEvent &inputEvent) {
    MetalApplication::inputEvent(inputEvent);
    _scene->updateInputEvent(inputEvent);
}

}
