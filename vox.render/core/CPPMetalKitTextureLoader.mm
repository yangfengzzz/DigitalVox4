/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Implementation of C++ MetalKit texture loader class wrapper
*/

#include <Metal/Metal.h>
#include "CPPMetalKitTextureLoader.hpp"
#include "CPPMetalDevice.hpp"

using namespace MTL;
using namespace MTK;

TextureLoader::TextureLoader(MTL::Device & device)
: m_objCObj([[MTKTextureLoader alloc] initWithDevice:device.objCObj()])
, m_device(&device)
{
    // Member initialization only
}


TextureLoader::~TextureLoader()
{
    m_objCObj = nil;
}

NSDictionary *textureLoaderDictionary(const TextureLoaderOptions & options)
{
    NSDictionary *textureLoaderOptions =
    @{
        MTKTextureLoaderOptionTextureUsage       : @(options.usage),
        MTKTextureLoaderOptionTextureStorageMode : @(options.storageMode),
      };

    return [textureLoaderOptions copyWithZone:nil];
}

MTL::Texture *TextureLoader::newTextureWithName(const char* name,
                                                float screenScaleFactor,
                                                const TextureLoaderOptions & options,
                                                CFErrorRef *error)
{
    NSString *nsname = [[NSString alloc] initWithUTF8String:name];
    NSError *nserror;

    id<MTLTexture> objCTexture = [m_objCObj newTextureWithName:nsname
                                                   scaleFactor:1.0
                                                        bundle:nil
                                                       options:textureLoaderDictionary(options)
                                                         error:&nserror];

    if(!objCTexture)
    {
        if(error)
        {
            *error = (__bridge CFErrorRef) nserror;
        }
        return nullptr;
    }

    Texture *texture = construct<Texture>(m_device->allocator(), objCTexture, *m_device);

    return texture;
}

MTL::Texture TextureLoader:: makeTexture(const char* name,
                                        float screenScaleFactor,
                                        const TextureLoaderOptions & options,
                                        CFErrorRef *error)
{
    NSString *nsname = [[NSString alloc] initWithUTF8String:name];
    NSError *nserror;

    id<MTLTexture> objCTexture = [m_objCObj newTextureWithName:nsname
                                                   scaleFactor:1.0
                                                        bundle:nil
                                                       options:textureLoaderDictionary(options)
                                                         error:&nserror];

    if(!objCTexture)
    {
        if(error)
        {
            *error = (__bridge CFErrorRef) nserror;
        }
    }

    return Texture(objCTexture, *m_device);
}

MTL::Texture *TextureLoader::newTextureWithContentsOfURL(const char* URLString,
                                                         const TextureLoaderOptions & options,
                                                         CFErrorRef *error)
{
    NSString *nsURLString = [[NSString alloc] initWithUTF8String:URLString];
    NSURL* URL = [[NSURL alloc] initWithString:nsURLString];
    NSError *nserror;

    id<MTLTexture> objCTexture = [m_objCObj newTextureWithContentsOfURL:URL
                                                                options:textureLoaderDictionary(options)
                                                                  error:&nserror];

    if(!objCTexture)
    {
        if(error)
        {
            *error = (__bridge CFErrorRef) nserror;
        }
        return nullptr;
    }

    Texture *texture = construct<Texture>(m_device->allocator(), objCTexture, *m_device);

    return texture;
}

MTL::Texture TextureLoader::makeTexture(const char* URLString,
                                        const TextureLoaderOptions & options,
                                        CFErrorRef *error)
{
    NSString *nsURLString = [[NSString alloc] initWithUTF8String:URLString];
    NSURL* URL = [[NSURL alloc] initWithString:nsURLString];
    NSError *nserror;

    id<MTLTexture> objCTexture = [m_objCObj newTextureWithContentsOfURL:URL
                                                                options:textureLoaderDictionary(options)
                                                                  error:&nserror];

    if(!objCTexture)
    {
        if(error)
        {
            *error = (__bridge CFErrorRef) nserror;
        }

        return Texture();
    }

    return Texture(objCTexture, *m_device);
}
