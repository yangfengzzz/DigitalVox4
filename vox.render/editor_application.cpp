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
#include "metal_helpers.h"

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
    auto colorPickerTextureDesc = CLONE_METAL_CUSTOM_DELETER(MTL::TextureDescriptor, MTL::TextureDescriptor::alloc()->init());
    colorPickerTextureDesc->setPixelFormat(_colorPickerFormat);
    colorPickerTextureDesc->setWidth(extent.width * scale);
    colorPickerTextureDesc->setHeight(extent.height * scale);
    colorPickerTextureDesc->setMipmapLevelCount(1);
    colorPickerTextureDesc->setTextureType(MTL::TextureType2D);
    colorPickerTextureDesc->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
    colorPickerTextureDesc->setStorageMode(MTL::StorageModeManaged);
    colorPickerTextureDesc->setPixelFormat(_colorPickerFormat);
    _colorPickerTexture = CLONE_METAL_CUSTOM_DELETER(MTL::Texture, _device->newTexture(colorPickerTextureDesc.get()));
    _colorPickerTexture->setLabel(NS::String::string("ColorPicker Texture", NS::StringEncoding::UTF8StringEncoding));
    _colorPickerPassDescriptor = CLONE_METAL_CUSTOM_DELETER(MTL::RenderPassDescriptor, MTL::RenderPassDescriptor::alloc()->init());
    _colorPickerPassDescriptor->colorAttachments()->object(0)->setTexture(_colorPickerTexture.get());
    _colorPickerPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionClear);
    _colorPickerPassDescriptor->depthAttachment()->setTexture(_renderContext->depthStencilTexture());
    _colorPickerRenderPass = std::make_unique<RenderPass>(*_library, *_colorPickerPassDescriptor);
    auto colorPickerSubpass = std::make_unique<ColorPickerSubpass>(_renderContext.get(), _scene.get(), _mainCamera);
    _colorPickerSubpass = colorPickerSubpass.get();
    _colorPickerRenderPass->addSubpass(std::move(colorPickerSubpass));
    
    return true;
}

void EditorApplication::update(float delta_time) {
    MetalApplication::update(delta_time);
    DebugFrameManager::getSingletonPtr()->flush();
    
    _scene->update(delta_time);
    
    auto commandBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::CommandBuffer, _commandQueue->commandBuffer());
    updateGPUTask(*commandBuffer);
    
    // The final pass can only render if a drawable is available, otherwise it needs to skip
    // rendering this frame.
    if (_renderContext->currentDrawable()) {
        // Render the lighting and composition pass
        _renderPassDescriptor->colorAttachments()->object(0)->setTexture(_renderContext->currentDrawableTexture());
        _renderPassDescriptor->depthAttachment()->setTexture(_renderContext->depthStencilTexture());
        _renderPassDescriptor->stencilAttachment()->setTexture(_renderContext->depthStencilTexture());
        _renderPass->draw(*commandBuffer, "Lighting & Composition Pass");
    }
    
    if (_needPick) {
        _colorPickerRenderPass->draw(*commandBuffer, "color Picker Pass");
        auto blit = CLONE_METAL_CUSTOM_DELETER(MTL::BlitCommandEncoder, commandBuffer->blitCommandEncoder());
        blit->synchronizeResource(_colorPickerTexture.get());
        blit->endEncoding();
    }
    
    // Finalize rendering here & push the command buffer to the GPU
    commandBuffer->presentDrawable(_renderContext->currentDrawable());
    commandBuffer->commit();
    commandBuffer->waitUntilCompleted();
    
    if (_needPick) {
        auto picker = _colorPickerSubpass->getObjectByColor(_readColorFromRenderTarget());
        pickFunctor(picker.first, picker.second);
        _needPick = false;
    }
}

bool EditorApplication::resize(uint32_t win_width, uint32_t win_height,
                               uint32_t fb_width, uint32_t fb_height) {
    ForwardApplication::resize(win_width, win_height, fb_width, fb_height);
    
    auto colorPickerTextureDesc = CLONE_METAL_CUSTOM_DELETER(MTL::TextureDescriptor, MTL::TextureDescriptor::alloc()->init());
    colorPickerTextureDesc->setPixelFormat(_colorPickerFormat);
    colorPickerTextureDesc->setWidth(fb_height);
    colorPickerTextureDesc->setHeight(fb_height);
    colorPickerTextureDesc->setMipmapLevelCount(1);
    colorPickerTextureDesc->setTextureType(MTL::TextureType2D);
    colorPickerTextureDesc->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
    colorPickerTextureDesc->setStorageMode(MTL::StorageModeManaged);
    colorPickerTextureDesc->setPixelFormat(_colorPickerFormat);
    _colorPickerTexture = CLONE_METAL_CUSTOM_DELETER(MTL::Texture, _device->newTexture(colorPickerTextureDesc.get()));
    _colorPickerTexture->setLabel(NS::String::string("ColorPicker Texture", NS::StringEncoding::UTF8StringEncoding));
    _colorPickerPassDescriptor->colorAttachments()->object(0)->setTexture(_colorPickerTexture.get());
    _colorPickerPassDescriptor->depthAttachment()->setTexture(_renderContext->depthStencilTexture());
    
    return true;
}

void EditorApplication::pick(float offsetX, float offsetY) {
    _needPick = true;
    _pickPos = Vector2F(offsetX, offsetY);
}

std::array<uint8_t, 4> EditorApplication::_readColorFromRenderTarget() {
    int clientWidth = _mainCamera->width();
    int clientHeight = _mainCamera->height();
    int canvasWidth = static_cast<int>(_colorPickerTexture->width());
    int canvasHeight = static_cast<int>(_colorPickerTexture->height());
    
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
    
    _colorPickerTexture->getBytes(pixel.data(), sizeof(uint8_t) * 4,
                                  MTL::Region(left, canvasHeight - bottom, 1, 1), 0);
    
    return pixel;
}

}
