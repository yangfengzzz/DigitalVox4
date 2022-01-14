/*
 See LICENSE folder for this sample’s licensing information.
 
 Abstract:
 Implementation of C++ MetalKit texture loader class wrapper
 */

#include <Metal/Metal.h>
#include "texture_loader.h"
#include "CPPMetalDevice.hpp"
#include <string>
#include <array>

using namespace MTL;

namespace vox {
TextureLoader::TextureLoader(MTL::Device &device)
: m_objCObj([[MTKTextureLoader alloc] initWithDevice:device.objCObj()]), m_device(&device) {
    // Member initialization only
}


TextureLoader::~TextureLoader() {
    m_objCObj = nil;
}

NSDictionary *textureLoaderDictionary(const TextureLoaderOptions &options) {
    NSDictionary *textureLoaderOptions =
    @{
        MTKTextureLoaderOptionTextureUsage: @(options.usage),
        MTKTextureLoaderOptionTextureStorageMode: @(options.storageMode),
    };
    
    return [textureLoaderOptions copyWithZone:nil];
}

MTL::Texture *TextureLoader::newTextureWithName(const char *name,
                                                float screenScaleFactor,
                                                const TextureLoaderOptions &options,
                                                CFErrorRef *error) {
    NSString *nsname = [[NSString alloc] initWithUTF8String:name];
    NSError *nserror;
    
    id <MTLTexture> objCTexture = [m_objCObj newTextureWithName:nsname
                                                    scaleFactor:1.0
                                                         bundle:nil
                                                        options:textureLoaderDictionary(options)
                                                          error:&nserror];
    
    if (!objCTexture) {
        if (error) {
            *error = (__bridge CFErrorRef) nserror;
        }
        return nullptr;
    }
    
    Texture *texture = construct<Texture>(m_device->allocator(), objCTexture, *m_device);
    
    return texture;
}

MTL::Texture TextureLoader::makeTexture(const char *name,
                                        float screenScaleFactor,
                                        const TextureLoaderOptions &options,
                                        CFErrorRef *error) {
    NSString *nsname = [[NSString alloc] initWithUTF8String:name];
    NSError *nserror;
    
    id <MTLTexture> objCTexture = [m_objCObj newTextureWithName:nsname
                                                    scaleFactor:1.0
                                                         bundle:nil
                                                        options:textureLoaderDictionary(options)
                                                          error:&nserror];
    
    if (!objCTexture) {
        if (error) {
            *error = (__bridge CFErrorRef) nserror;
        }
    }
    
    return Texture(objCTexture, *m_device);
}

MTL::Texture *TextureLoader::newTextureWithContentsOfURL(const char *URLString,
                                                         const TextureLoaderOptions &options,
                                                         CFErrorRef *error) {
    NSString *nsURLString = [[NSString alloc] initWithUTF8String:URLString];
    NSURL *URL = [[NSURL alloc] initWithString:nsURLString];
    NSError *nserror;
    
    id <MTLTexture> objCTexture = [m_objCObj newTextureWithContentsOfURL:URL
                                                                 options:textureLoaderDictionary(options)
                                                                   error:&nserror];
    
    if (!objCTexture) {
        if (error) {
            *error = (__bridge CFErrorRef) nserror;
        }
        return nullptr;
    }
    
    Texture *texture = construct<Texture>(m_device->allocator(), objCTexture, *m_device);
    
    return texture;
}

MTL::Texture TextureLoader::makeTexture(const char *URLString,
                                        const TextureLoaderOptions &options,
                                        CFErrorRef *error) {
    NSString *nsURLString = [[NSString alloc] initWithUTF8String:URLString];
    NSURL *URL = [[NSURL alloc] initWithString:nsURLString];
    NSError *nserror;
    
    id <MTLTexture> objCTexture = [m_objCObj newTextureWithContentsOfURL:URL
                                                                 options:textureLoaderDictionary(options)
                                                                   error:&nserror];
    
    if (!objCTexture) {
        if (error) {
            *error = (__bridge CFErrorRef) nserror;
        }
        
        return Texture();
    }
    
    return Texture(objCTexture, *m_device);
}

MTL::Texture TextureLoader::loadCubeTexture(const std::string &path,
                                            const std::array<std::string, 6> &imageName,
                                            bool isTopLeft) {
    NSString *pathName = [[NSString alloc] initWithUTF8String:path.c_str()];
    NSString *textureName1 = [[NSString alloc] initWithUTF8String:imageName[0].c_str()];
    NSString *textureName2 = [[NSString alloc] initWithUTF8String:imageName[1].c_str()];
    NSString *textureName3 = [[NSString alloc] initWithUTF8String:imageName[2].c_str()];
    NSString *textureName4 = [[NSString alloc] initWithUTF8String:imageName[3].c_str()];
    NSString *textureName5 = [[NSString alloc] initWithUTF8String:imageName[4].c_str()];
    NSString *textureName6 = [[NSString alloc] initWithUTF8String:imageName[5].c_str()];
    
    NSMutableArray<NSString *> *imageNames = [[NSMutableArray alloc] init];
    [imageNames addObject:textureName1];
    [imageNames addObject:textureName2];
    [imageNames addObject:textureName3];
    [imageNames addObject:textureName4];
    [imageNames addObject:textureName5];
    [imageNames addObject:textureName6];
    
    MDLTexture *mdlTexture = [MDLTexture textureCubeWithImagesNamed:imageNames bundle:[NSBundle bundleWithPath:pathName]];
    MTKTextureLoaderOrigin origin = MTKTextureLoaderOriginTopLeft;
    if (!isTopLeft) {
        origin = MTKTextureLoaderOriginBottomLeft;
    }
    
    NSDictionary < MTKTextureLoaderOption, id > *options = @{
        MTKTextureLoaderOptionOrigin: origin,
        MTKTextureLoaderOptionGenerateMipmaps: [NSNumber numberWithBool:FALSE],
        MTKTextureLoaderOptionTextureUsage: [NSNumber numberWithUnsignedLong:MTLTextureUsageShaderRead]
    };
    NSError *error = nil;
    id <MTLTexture> mtlTexture = [m_objCObj newTextureWithMDLTexture:mdlTexture options:options error:&error];
    if (error != nil) {
        NSLog(@"Error: failed to create MTLTexture: %@", error);
    }
    
    return Texture(mtlTexture, *m_device);
}

MTL::Texture TextureLoader::loadTexture(const std::string &path, const std::string &imageName, bool isTopLeft) {
    NSString *pathName = [[NSString alloc] initWithUTF8String:path.c_str()];
    NSString *textureName = [[NSString alloc] initWithUTF8String:imageName.c_str()];
    NSURL *url = [[NSBundle bundleWithPath:pathName] URLForResource:textureName withExtension:nil];
    
    MTKTextureLoaderOrigin origin = MTKTextureLoaderOriginTopLeft;
    if (!isTopLeft) {
        origin = MTKTextureLoaderOriginBottomLeft;
    }
    
    NSDictionary < MTKTextureLoaderOption, id > *options = @{
        MTKTextureLoaderOptionOrigin: origin,
        MTKTextureLoaderOptionGenerateMipmaps: [NSNumber numberWithBool:TRUE]
    };
    
    NSError *error = nil;
    id <MTLTexture> texture = [m_objCObj newTextureWithContentsOfURL:url
                                                             options:options error:&error];
    if (error != nil) {
        NSLog(@"Error: failed to create MTLTexture: %@", error);
    }
    
    return Texture(texture, *m_device);
}

}
