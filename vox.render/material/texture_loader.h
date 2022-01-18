//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cpp_mtl_KitTextureLoader_hpp
#define cpp_mtl_KitTextureLoader_hpp

#include "cpp_mtl_implementation.h"
#include "cpp_mtl_texture.h"


namespace MTL {
class Device;
}

namespace vox {
struct TextureLoaderOptions {
    MTL::TextureUsage usage;
    MTL::StorageMode storageMode;
    
    TextureLoaderOptions() :
    usage(MTL::TextureUsageUnknown),
    storageMode(MTL::StorageModePrivate) {
        // Member initialization only
    }
};

class TextureLoader {
public:
    TextureLoader() = delete;
    
    explicit TextureLoader(MTL::Device &device);
    
    CPP_METAL_VIRTUAL ~TextureLoader();
    
    MTL::TexturePtr makeTexture(const char *name,
                                float catalogScaleFactor,
                                const TextureLoaderOptions &options,
                                CFErrorRef *error = nullptr);
    
    MTL::TexturePtr makeTexture(const char *URLString,
                                const TextureLoaderOptions &options,
                                CFErrorRef *error = nullptr);
    
public:
    MTL::TexturePtr loadCubeTexture(const std::string &path,
                                    const std::array<std::string, 6> &imageName,
                                    bool isTopLeft);
    
    MTL::Texture loadTexture(const std::string &path,
                             const std::string &imageName,
                             bool isTopLeft);
    
private:
    cpp_mtl_internal::TextureLoader _objCObj;
    
    MTL::Device *_device;
};


} // namespace vox

#endif // cpp_mtl_KitTextureLoader_hpp
