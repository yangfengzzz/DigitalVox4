/*
 See LICENSE folder for this sample’s licensing information.
 
 Abstract:
 Header for C++ MetalKit texture loader class wrapper
 */

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
    cpp_mtl_internal::TextureLoader m_objCObj;
    
    MTL::Device *m_device;
};


} // namespace vox

#endif // cpp_mtl_KitTextureLoader_hpp
