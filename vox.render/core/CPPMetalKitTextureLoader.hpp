/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for C++ MetalKit texture loader class wrapper
*/

#ifndef CPPMetalKitTextureLoader_hpp
#define CPPMetalKitTextureLoader_hpp

#include "CPPMetalImplementation.hpp"
#include "CPPMetalTexture.hpp"


namespace MTL
{

class Device;

}


namespace MTK
{


struct TextureLoaderOptions {

    MTL::TextureUsage usage;
    MTL::StorageMode  storageMode;

    TextureLoaderOptions() :
    usage(MTL::TextureUsageUnknown),
    storageMode(MTL::StorageModePrivate)
    {
        // Member initialization only
    }

};

class TextureLoader
{

public:

    TextureLoader() = delete;

    explicit TextureLoader(MTL::Device & device);

    CPP_METAL_VIRTUAL ~TextureLoader();

    MTL::Texture *newTextureWithName(const char* name,
                                     float catalogScaleFactor,
                                     const TextureLoaderOptions & options,
                                     CFErrorRef *error = nullptr);

    MTL::Texture makeTexture(const char* name,
                             float screenScaleFactor,
                             const TextureLoaderOptions & options,
                             CFErrorRef *error = nullptr);

    MTL::Texture *newTextureWithContentsOfURL(const char* URLString,
                                              const TextureLoaderOptions & options,
                                              CFErrorRef *error = nullptr);

    MTL::Texture makeTexture(const char* URLString,
                             const TextureLoaderOptions & options,
                             CFErrorRef *error = nullptr);

private:

    CPPMetalInternal::TextureLoader m_objCObj;

    MTL::Device *m_device;

};


} // namespace MTK

#endif // CPPMetalKitTextureLoader_hpp
