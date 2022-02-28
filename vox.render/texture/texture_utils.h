//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef texture_utils_hpp
#define texture_utils_hpp

#include <Metal/Metal.hpp>
#include <vector>

namespace vox {
class TextureUtils {
public:
    // n -> 1
    static void buildTextureArray(const std::vector<std::shared_ptr<MTL::Texture>>::iterator &texturesBegin,
                                  const std::vector<std::shared_ptr<MTL::Texture>>::iterator &texturesEnd,
                                  uint32_t width, uint32_t height,
                                  std::shared_ptr<MTL::Texture>& textureArray,
                                  MTL::CommandBuffer& commandBuffer);
    
    // 6n -> 1
    static void buildCubeTextureArray(const std::vector<std::shared_ptr<MTL::Texture>>::iterator &texturesBegin,
                                      const std::vector<std::shared_ptr<MTL::Texture>>::iterator &texturesEnd,
                                      uint32_t width, uint32_t height,
                                      std::shared_ptr<MTL::Texture>& textureArray,
                                      MTL::CommandBuffer& commandBuffer);
};

}

#endif /* texture_utils_hpp */
