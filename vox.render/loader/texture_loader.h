//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cpp_mtl_KitTextureLoader_hpp
#define cpp_mtl_KitTextureLoader_hpp

#include <Metal/Metal.hpp>

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
    
public:
    MTL::TexturePtr buildTexture(MTL::TextureDescriptor descriptor);
    
    MTL::TexturePtr buildTexture(int width, int height, MTL::PixelFormat pixelFormat,
                                 int usage = MTL::TextureUsageShaderRead | MTL::TextureUsageRenderTarget,
                                 MTL::StorageMode storageMode = MTL::StorageModePrivate);
    
    MTL::TexturePtr buildCubeTexture(int size, MTL::PixelFormat pixelFormat,
                                     int usage = MTL::TextureUsageShaderRead | MTL::TextureUsageRenderTarget,
                                     MTL::StorageMode storageMode = MTL::StorageModePrivate);
    
    MTL::TexturePtr loadTexture(const std::string &path,
                                const std::string &imageName,
                                bool isTopLeft = true);
    
    MTL::TexturePtr loadCubeTexture(const std::string &path,
                                    const std::array<std::string, 6> &imageName,
                                    bool isTopLeft = true);
    
    MTL::TexturePtr loadTextureArray(const std::string &path, const std::vector<std::string> &textureNames);
    
    MTL::TexturePtr createTextureArray(const std::vector<MTL::TexturePtr> &textures);
    
public:
    MTL::TexturePtr createIrradianceTexture(const std::string &path,
                                            const std::array<std::string, 6> &imageName, float roughness = 0.6,
                                            bool isDebugger = false, bool isTopLeft = true);
    
    // red 9; green 9; blue 9;
    std::array<float, 27> createSphericalHarmonicsCoefficients(const std::string &path,
                                                               const std::array<std::string, 6> &imageName);
    
    MTL::TexturePtr createSpecularTexture(const std::string &path,
                                          const std::array<std::string, 6> &imageName,
                                          bool isDebugger = false, bool isTopLeft = true);
    
    MTL::TexturePtr createBRDFLookupTable();
    
    MTL::TexturePtr createMetallicRoughnessTexture(const std::string &path, const std::string &metallic,
                                                   const std::string &roughness, bool isTopLeft = true);
    
public:
    MTL::TexturePtr makeTexture(const char *name,
                                float catalogScaleFactor,
                                const TextureLoaderOptions &options,
                                CFErrorRef *error = nullptr);
    
    MTL::TexturePtr makeTexture(const char *URLString,
                                const TextureLoaderOptions &options,
                                CFErrorRef *error = nullptr);
    
private:
    cpp_mtl_internal::TextureLoader _objCObj;
    MTL::CommandQueue _commandQueue;
    MTL::Library _library;
    MTL::Device *_device;
};


} // namespace vox

#endif // cpp_mtl_KitTextureLoader_hpp
