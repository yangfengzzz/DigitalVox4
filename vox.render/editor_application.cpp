//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "editor_application.h"
#include "rendering/subpasses/forward_subpass.h"
#include "rendering/subpasses/color_picker_subpass.h"
#include "engine.h"
#include "camera.h"

namespace vox {
EditorApplication::~EditorApplication() {
    _renderPass.reset();
    delete _debugFrameManager;
}

bool EditorApplication::prepare(Engine &engine) {
    ForwardApplication::prepare(engine);
    
    _debugFrameManager = new DebugFrameManager(_scene->createRootEntity());
    
    auto extent = engine.window().extent();
    auto scale = engine.window().contentScaleFactor();
    
    _colorPickerFormat = MTL::PixelFormatBGRA8Unorm;
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
    DebugFrameManager::getSingletonPtr()->flush();
    
    _scene->update(delta_time);
    _scene->updateShaderData();
    
    MTL::CommandBuffer commandBuffer = _commandQueue.commandBuffer();
    _shadowManager->draw(commandBuffer);

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
    
    if (_needPick) {
        auto picker = _colorPickerSubpass->getObjectByColor(_readColorFromRenderTarget());
        pickFunctor(picker.first, picker.second);
        _needPick = false;
    }
}

bool EditorApplication::resize(uint32_t win_width, uint32_t win_height,
                               uint32_t fb_width, uint32_t fb_height) {
    ForwardApplication::resize(win_width, win_height, fb_width, fb_height);
    
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
    
    return true;
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
