/*
 See LICENSE folder for this sample’s licensing information.
 
 Abstract:
 Header for C++ MetalKit texture loader class wrapper
 */

#ifndef CPPMetalKitTextureLoader_hpp
#define CPPMetalKitTextureLoader_hpp

#include "CPPMetalImplementation.hpp"
#include "CPPMetalTexture.hpp"


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
    CPPMetalInternal::TextureLoader m_objCObj;
    
    MTL::Device *m_device;
};


} // namespace vox

#endif // CPPMetalKitTextureLoader_hpp
