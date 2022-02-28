//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "texture_utils.h"
#include "metal_helpers.h"

namespace vox {
void TextureUtils::buildTextureArray(const std::vector<std::shared_ptr<MTL::Texture>>::iterator &texturesBegin,
                                     const std::vector<std::shared_ptr<MTL::Texture>>::iterator &texturesEnd,
                                     uint32_t width, uint32_t height,
                                     MTL::Texture& textureArray,
                                     MTL::CommandBuffer& commandBuffer) {
    auto blit = CLONE_METAL_CUSTOM_DELETER(MTL::BlitCommandEncoder, commandBuffer.blitCommandEncoder());
    MTL::Origin origin = MTL::Origin(0, 0, 0);
    MTL::Size size = MTL::Size(textureArray.width(), textureArray.height(), 1);
    
    for (auto iter = texturesBegin; iter < texturesEnd; iter++) {
        blit->copyFromTexture(iter->get(), 0, 0, origin, size,
                              &textureArray, iter - texturesBegin, 0, origin);
    }
    blit->endEncoding();
}

void TextureUtils::buildCubeTextureArray(const std::vector<std::shared_ptr<MTL::Texture>>::iterator &texturesBegin,
                                         const std::vector<std::shared_ptr<MTL::Texture>>::iterator &texturesEnd,
                                         uint32_t width, uint32_t height,
                                         MTL::Texture& textureArray,
                                         MTL::CommandBuffer& commandBuffer) {
    auto blit = CLONE_METAL_CUSTOM_DELETER(MTL::BlitCommandEncoder, commandBuffer.blitCommandEncoder());
    int destinationSlice = 0;
    for (auto iter = texturesBegin; iter < texturesEnd; iter++) {
        blit->copyFromTexture(iter->get(), 0, 0, &textureArray, destinationSlice, 0, 6, 1);
        destinationSlice += 6;
    }
    blit->endEncoding();
}

}
