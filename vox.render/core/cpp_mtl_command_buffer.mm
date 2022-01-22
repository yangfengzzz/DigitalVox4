//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cpp_mtl_command_buffer.h"
#include "cpp_mtl_render_command_encoder.h"
#include "cpp_mtl_internal_macros.h"
#include "cpp_mtl_render_pass.h"
#include "cpp_mtl_device.h"
#include "cpp_mtl_drawable.h"
#include <Metal/Metal.h>
#include <array>

using namespace MTL;

CommandBufferHandler::~CommandBufferHandler() {
    // Pure virtual class destructor
}

CPP_METAL_CONSTRUCTOR_IMPLEMENTATION(CommandBuffer);

CPP_METAL_COPY_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(CommandBuffer);

CommandBuffer::~CommandBuffer() {
    m_objCObj = nil;
}

bool CommandBuffer::operator==(const CommandBuffer &rhs) {
    return [m_objCObj isEqual:rhs.m_objCObj];
}

const Device* CommandBuffer::device() const {
    return m_device;
}

CPP_METAL_READWRITE_LABEL_PROPERTY_IMPLEMENTATION(CommandBuffer)

RenderCommandEncoder CommandBuffer::renderCommandEncoderWithDescriptor(const RenderPassDescriptor &descriptor) const {
    const id <MTLRenderCommandEncoder> objCObj = [m_objCObj renderCommandEncoderWithDescriptor:descriptor.objCObj()];
    
    return RenderCommandEncoder(objCObj, *m_device);
}

void CommandBuffer::commit() {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    [m_objCObj commit];
}

void CommandBuffer::presentDrawable(Drawable &drawable) {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    [m_objCObj presentDrawable:drawable.objCObj()];
    drawable.invalidate();
}

void CommandBuffer::waitUntilCompleted() {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    [m_objCObj waitUntilCompleted];
}

void CommandBuffer::addCompletedHandler(CommandBufferHandler &completedHandler) {
    Device *device = m_device;
    [m_objCObj addCompletedHandler:^(id <MTLCommandBuffer> _Nonnull objCCommandBuffer) {
        CommandBuffer commandBuffer(objCCommandBuffer, *device);
        completedHandler(commandBuffer);
    }];
}

void CommandBuffer::addScheduledHandler(CommandBufferHandler &scheduledHandler) {
    Device *device = m_device;
    [m_objCObj addScheduledHandler:^(id <MTLCommandBuffer> _Nonnull objCCommandBuffer) {
        CommandBuffer commandBuffer(objCCommandBuffer, *device);
        scheduledHandler(commandBuffer);
    }];
}

//MARK: - Blit
void CommandBuffer::synchronizeResource(MTL::Texture& resource) {
    auto blit = [m_objCObj blitCommandEncoder];
    [blit synchronizeResource:resource.objCObj()];
    [blit endEncoding];
}

MTL::TexturePtr CommandBuffer::createTextureArray(const std::vector<MTL::TexturePtr>::iterator &texturesBegin,
                                                  const std::vector<MTL::TexturePtr>::iterator &texturesEnd,
                                                  MTL::TexturePtr packedTextures) {
    if (packedTextures == nullptr || packedTextures->arrayLength() != texturesEnd - texturesBegin) {
        MTLTextureDescriptor *descriptor = [[MTLTextureDescriptor alloc] init];
        descriptor.textureType = MTLTextureType2DArray;
        descriptor.pixelFormat = (MTLPixelFormat)(*texturesBegin)->pixelFormat();
        descriptor.width = (*texturesBegin)->width();
        descriptor.height = (*texturesBegin)->height();
        descriptor.arrayLength = texturesEnd - texturesBegin;
        descriptor.storageMode = MTLStorageModePrivate;
        
        packedTextures = std::make_shared<Texture>([m_device->objCObj() newTextureWithDescriptor:descriptor], *m_device);
    }
    
    auto blitEncoder = [m_objCObj blitCommandEncoder];
    MTLOrigin origin = MTLOrigin{.x =  0, .y =  0, .z =  0};
    MTLSize size = MTLSize{.width =  packedTextures->width(),
        .height =  packedTextures->height(), .depth = 1};
    for (auto iter = texturesBegin; iter < texturesEnd; iter++) {
        [blitEncoder copyFromTexture:(*iter)->objCObj() sourceSlice:0 sourceLevel:0 sourceOrigin:origin sourceSize:size
                           toTexture:packedTextures->objCObj() destinationSlice:iter - texturesBegin destinationLevel:0 destinationOrigin:origin];
    }
    [blitEncoder endEncoding];
    return packedTextures;
}

MTL::TexturePtr CommandBuffer::createCubeTextureArray(const std::vector<MTL::TexturePtr>::iterator &texturesBegin,
                                                      const std::vector<MTL::TexturePtr>::iterator &texturesEnd,
                                                      MTL::TexturePtr packedTextures) {
    if (packedTextures == nullptr || packedTextures->arrayLength() != texturesEnd - texturesBegin) {
        MTLTextureDescriptor *descriptor = [[MTLTextureDescriptor alloc] init];
        descriptor.textureType = MTLTextureTypeCubeArray;
        descriptor.pixelFormat = (MTLPixelFormat)(*texturesBegin)->pixelFormat();
        descriptor.width = (*texturesBegin)->width();
        descriptor.height = (*texturesBegin)->height();
        descriptor.arrayLength = texturesEnd - texturesBegin;
        descriptor.storageMode = MTLStorageModePrivate;
        
        packedTextures = std::make_shared<Texture>([m_device->objCObj() newTextureWithDescriptor:descriptor], *m_device);
    }
    
    int destinationSlice = 0;
    auto blitEncoder = [m_objCObj blitCommandEncoder];
    for (auto iter = texturesBegin; iter < texturesEnd; iter++) {
        [blitEncoder copyFromTexture:(*iter)->objCObj() sourceSlice:0 sourceLevel:0
                           toTexture:packedTextures->objCObj() destinationSlice:destinationSlice destinationLevel:0
                          sliceCount:6 levelCount:1];
        destinationSlice += 6;
    }
    [blitEncoder endEncoding];
    return packedTextures;
}

MTL::TexturePtr CommandBuffer::createAtlas(const std::array<MTL::TexturePtr, 4> &textures,
                                           MTL::TexturePtr packedTextures) {
    auto blitEncoder = [m_objCObj blitCommandEncoder];
    MTLOrigin origin = MTLOrigin{.x =  0, .y =  0, .z =  0};
    MTLOrigin sourceOrigin = origin;
    MTLSize sourceSize = MTLSize{.width = textures[0]->width(),
        .height = textures[0]->height(), .depth = 1};
    [blitEncoder copyFromTexture:textures[0]->objCObj() sourceSlice:0 sourceLevel:0 sourceOrigin:sourceOrigin sourceSize:sourceSize
                       toTexture:packedTextures->objCObj() destinationSlice:0 destinationLevel:0 destinationOrigin:origin];
    origin.x = textures[0]->width();
    [blitEncoder copyFromTexture:textures[1]->objCObj() sourceSlice:0 sourceLevel:0 sourceOrigin:sourceOrigin sourceSize:sourceSize
                       toTexture:packedTextures->objCObj() destinationSlice:0 destinationLevel:0 destinationOrigin:origin];
    origin.x = 0;
    origin.y = textures[0]->height();
    [blitEncoder copyFromTexture:textures[2]->objCObj() sourceSlice:0 sourceLevel:0 sourceOrigin:sourceOrigin sourceSize:sourceSize
                       toTexture:packedTextures->objCObj() destinationSlice:0 destinationLevel:0 destinationOrigin:origin];
    origin.x = textures[0]->width();
    origin.y = textures[0]->height();
    [blitEncoder copyFromTexture:textures[3]->objCObj() sourceSlice:0 sourceLevel:0 sourceOrigin:sourceOrigin sourceSize:sourceSize
                       toTexture:packedTextures->objCObj() destinationSlice:0 destinationLevel:0 destinationOrigin:origin];
    
    [blitEncoder endEncoding];
    return packedTextures;
}

MTL::TexturePtr CommandBuffer::createCubeAtlas(const std::array<MTL::TexturePtr, 6> &textures,
                                               MTL::TexturePtr packedTextures) {
    auto blitEncoder = [m_objCObj blitCommandEncoder];
    [blitEncoder copyFromTexture:textures[0]->objCObj() sourceSlice:0 sourceLevel:0
                       toTexture:packedTextures->objCObj() destinationSlice:0 destinationLevel:0 sliceCount:1 levelCount:1];
    [blitEncoder copyFromTexture:textures[1]->objCObj() sourceSlice:0 sourceLevel:0
                       toTexture:packedTextures->objCObj() destinationSlice:1 destinationLevel:0 sliceCount:1 levelCount:1];
    [blitEncoder copyFromTexture:textures[2]->objCObj() sourceSlice:0 sourceLevel:0
                       toTexture:packedTextures->objCObj() destinationSlice:2 destinationLevel:0 sliceCount:1 levelCount:1];
    [blitEncoder copyFromTexture:textures[3]->objCObj() sourceSlice:0 sourceLevel:0
                       toTexture:packedTextures->objCObj() destinationSlice:3 destinationLevel:0 sliceCount:1 levelCount:1];
    [blitEncoder copyFromTexture:textures[4]->objCObj() sourceSlice:0 sourceLevel:0
                       toTexture:packedTextures->objCObj() destinationSlice:4 destinationLevel:0 sliceCount:1 levelCount:1];
    [blitEncoder copyFromTexture:textures[5]->objCObj() sourceSlice:0 sourceLevel:0
                       toTexture:packedTextures->objCObj() destinationSlice:5 destinationLevel:0 sliceCount:1 levelCount:1];
    [blitEncoder endEncoding];
    return packedTextures;
}
