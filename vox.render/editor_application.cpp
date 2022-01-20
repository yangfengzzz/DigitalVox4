//
//  editor_application.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/20.
//

#include "editor_application.h"
#include "rendering/subpasses/forward_subpass.h"
#include "rendering/subpasses/color_picker_subpass.h"
#include "engine.h"
#include "camera.h"

namespace vox {
EditorApplication::~EditorApplication() {
    _renderPass.reset();
}

bool EditorApplication::prepare(Engine &engine) {
    MetalApplication::prepare(engine);
    
    _scene = std::make_unique<Scene>();
    
    auto extent = engine.window().extent();
    auto scale = engine.window().contentScaleFactor();
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
    
    _colorPickerFormat = MTL::PixelFormatRGBA8Unorm;
    MTL::TextureDescriptor colorPickerTextureDesc;
    colorPickerTextureDesc.pixelFormat(_colorPickerFormat);
    colorPickerTextureDesc.width(extent.width * scale);
    colorPickerTextureDesc.height(extent.height * scale);
    colorPickerTextureDesc.mipmapLevelCount(1);
    colorPickerTextureDesc.textureType(MTL::TextureType2D);
    colorPickerTextureDesc.usage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
    colorPickerTextureDesc.storageMode(MTL::StorageModeManaged);
    colorPickerTextureDesc.pixelFormat(_colorPickerFormat);
    _colorPickerTexture = _device->makeTexture(colorPickerTextureDesc);
    _colorPickerTexture.label("ColorPicker Texture");
    _colorPickerPassDescriptor.colorAttachments[0].texture(_colorPickerTexture);
    _colorPickerPassDescriptor.depthAttachment.loadAction(MTL::LoadActionClear);
    _colorPickerPassDescriptor.depthAttachment.texture(*_renderView->depthStencilTexture());
    _colorPickerRenderPass = std::make_unique<RenderPass>(_library, &_colorPickerPassDescriptor);
    auto colorPickerSubpass = std::make_unique<ColorPickerSubpass>(_renderView.get(), _scene.get(), _mainCamera);
    _colorPickerSubpass = colorPickerSubpass.get();
    _colorPickerRenderPass->addSubpass(std::move(colorPickerSubpass));

    return true;
}

void EditorApplication::update(float delta_time) {
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
    
    if (_needPick) {
        _colorPickerRenderPass->draw(commandBuffer, "color Picker Pass");
        commandBuffer.synchronizeResource(_colorPickerTexture);
    }
    
    // Finalize rendering here & push the command buffer to the GPU
    commandBuffer.commit();
    commandBuffer.waitUntilCompleted();
    drawable->present();
    
    auto picker = _colorPickerSubpass->getObjectByColor(_readColorFromRenderTarget());
    pickFunctor(picker.first, picker.second);
}

bool EditorApplication::resize(uint32_t win_width, uint32_t win_height,
                                uint32_t fb_width, uint32_t fb_height) {
    MetalApplication::resize(win_width, win_height, fb_width, fb_height);
    
    MTL::TextureDescriptor colorPickerTextureDesc;
    colorPickerTextureDesc.pixelFormat(_colorPickerFormat);
    colorPickerTextureDesc.width(fb_height);
    colorPickerTextureDesc.height(fb_height);
    colorPickerTextureDesc.mipmapLevelCount(1);
    colorPickerTextureDesc.textureType(MTL::TextureType2D);
    colorPickerTextureDesc.usage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
    colorPickerTextureDesc.storageMode(MTL::StorageModeManaged);
    colorPickerTextureDesc.pixelFormat(_colorPickerFormat);
    _colorPickerTexture = _device->makeTexture(colorPickerTextureDesc);
    _colorPickerTexture.label("ColorPicker Texture");
    _colorPickerPassDescriptor.colorAttachments[0].texture(_colorPickerTexture);
    _colorPickerPassDescriptor.depthAttachment.texture(*_renderView->depthStencilTexture());
    
    _scene->updateSize(win_width, win_height, fb_width, fb_height);
    _mainCamera->resize(win_width, win_height);
    return true;
}

void EditorApplication::inputEvent(const InputEvent &inputEvent) {
    MetalApplication::inputEvent(inputEvent);
    _scene->updateInputEvent(inputEvent);
}

void EditorApplication::pick(float offsetX, float offsetY) {
    _needPick = true;
    _pickPos = Vector2F(offsetX, offsetY);
}

std::array<uint8_t, 4> EditorApplication::_readColorFromRenderTarget() {
    int clientWidth = _mainCamera->width();
    int clientHeight = _mainCamera->height();
    int canvasWidth = static_cast<int>(_colorPickerTexture.width());
    int canvasHeight = static_cast<int>(_colorPickerTexture.height());
    
    const auto px = (_pickPos.x / clientWidth) * canvasWidth;
    const auto py = (_pickPos.y / clientHeight) * canvasHeight;
    
    const auto viewport = _mainCamera->viewport();
    const auto viewWidth = (viewport.z - viewport.x) * canvasWidth;
    const auto viewHeight = (viewport.w - viewport.y) * canvasHeight;
    
    const auto nx = (px - viewport.x) / viewWidth;
    const auto ny = (py - viewport.y) / viewHeight;
    const auto left = std::floor(nx * (canvasWidth - 1));
    const auto bottom = std::floor((1 - ny) * (canvasHeight - 1));
    std::array<uint8_t, 4> pixel;
    
    _colorPickerTexture.getBytes(pixel.data(), sizeof(uint8_t) * 4,
                                 MTL::regionMake2D(left, canvasHeight - bottom, 1, 1), 0);
    
    return pixel;
}

}
