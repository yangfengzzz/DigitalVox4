//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <Metal/Metal.h>
#include "texture_loader.h"
#include "core/cpp_mtl_assert.h"
#include <string>
#include <iostream>
#include <vector>
#include <array>

using namespace MTL;

namespace vox {
TextureLoader::TextureLoader(MTL::Device &device)
: _objCObj([[MTKTextureLoader alloc] initWithDevice:device.objCObj()]),
_device(&device) {
    _commandQueue = _device->makeCommandQueue();
    
    CFErrorRef error = nullptr;
    CFURLRef libraryURL = CFBundleCopyResourceURL( CFBundleGetMainBundle() , CFSTR("vox.shader"), CFSTR("metallib"), nullptr);
    _library = _device->makeLibrary(libraryURL, &error);
    MTLAssert(!error, error, "Could not load Metal shader library");
    CFRelease(libraryURL);
}

TextureLoader::~TextureLoader() {
    _objCObj = nil;
}

MTL::TexturePtr TextureLoader::buildTexture(MTL::TextureDescriptor descriptor) {
    return MTL::TexturePtr(_device->newTextureWithDescriptor(descriptor));
}

MTL::TexturePtr TextureLoader::buildTexture(int width, int height, MTL::PixelFormat pixelFormat,
                                            int usage, MTL::StorageMode storageMode) {
    MTLTextureDescriptor *descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:(MTLPixelFormat)pixelFormat
                                                                                          width:width height:height
                                                                                      mipmapped:false];
    descriptor.usage = usage;
    descriptor.storageMode = (MTLStorageMode)storageMode;
    return std::make_shared<Texture>([_device->objCObj() newTextureWithDescriptor:descriptor], *_device);
}

MTL::TexturePtr TextureLoader::buildCubeTexture(int size, MTL::PixelFormat pixelFormat,
                                                int usage, MTL::StorageMode storageMode) {
    MTLTextureDescriptor *descriptor = [MTLTextureDescriptor textureCubeDescriptorWithPixelFormat:(MTLPixelFormat)pixelFormat
                                                                                             size:size
                                                                                        mipmapped:false];
    descriptor.usage = usage;
    descriptor.storageMode = (MTLStorageMode)storageMode;
    return std::make_shared<Texture>([_device->objCObj() newTextureWithDescriptor:descriptor], *_device);
}

MTL::TexturePtr TextureLoader::loadTexture(const std::string &path, const std::string &imageName, bool isTopLeft) {
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
    id <MTLTexture> texture = [_objCObj newTextureWithContentsOfURL:url
                                                            options:options error:&error];
    if (error != nil) {
        NSLog(@"Error: failed to create MTLTexture: %@", error);
    }
    
    return std::make_shared<Texture>(texture, *_device);
}

TexturePtr TextureLoader::loadCubeTexture(const std::string &path,
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
    id <MTLTexture> mtlTexture = [_objCObj newTextureWithMDLTexture:mdlTexture options:options error:&error];
    if (error != nil) {
        NSLog(@"Error: failed to create MTLTexture: %@", error);
    }
    
    return std::make_shared<Texture>(mtlTexture, *_device);
}

TexturePtr TextureLoader::loadTextureArray(const std::string &path, const std::vector<std::string> &textureNames) {
    std::vector<TexturePtr> textures;
    for (const auto &name: textureNames) {
        auto texture = loadTexture(path, name);
        if (texture != nullptr) {
            textures.push_back(texture);
        }
    }
    return createTextureArray(textures);
}

TexturePtr TextureLoader::createTextureArray(const std::vector<TexturePtr> &textures) {
    MTLTextureDescriptor *descriptor = [[MTLTextureDescriptor alloc] init];
    descriptor.textureType = MTLTextureType2DArray;
    descriptor.pixelFormat = (MTLPixelFormat)textures[0]->pixelFormat();
    descriptor.width = textures[0]->width();
    descriptor.height = textures[0]->height();
    descriptor.arrayLength = textures.size();
    descriptor.storageMode = MTLStorageModePrivate;
    
    auto arrayTexture = [_device->objCObj() newTextureWithDescriptor:descriptor];
    auto commandBuffer = [_commandQueue.objCObj() commandBuffer];
    auto blitEncoder = [commandBuffer blitCommandEncoder];
    MTLOrigin origin = MTLOrigin{.x =  0, .y =  0, .z =  0};
    MTLSize size = MTLSize{.width =  arrayTexture.width,
        .height =  arrayTexture.height, .depth = 1};
    for (size_t index = 0; index < textures.size(); index++) {
        [blitEncoder copyFromTexture:textures[index]->objCObj() sourceSlice:0 sourceLevel:0 sourceOrigin:origin sourceSize:size
                           toTexture:arrayTexture destinationSlice:index destinationLevel:0 destinationOrigin:origin];
    }
    [blitEncoder endEncoding];
    [commandBuffer commit];
    [commandBuffer waitUntilCompleted];
    
    return std::make_shared<Texture>(arrayTexture, *_device);
}

//MARK: - PBR Utility
TexturePtr TextureLoader::createIrradianceTexture(const std::string &path,
                                                  const std::array<std::string, 6> &imageName,
                                                  float roughness, bool isDebugger, bool isTopLeft) {
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
    
    auto irradianceTexture = [MDLTexture irradianceTextureCubeWithTexture:mdlTexture
                                                                     name:NULL dimensions:simd_make_int2(32, 32) roughness:roughness];
    
    MTKTextureLoaderOrigin origin = MTKTextureLoaderOriginTopLeft;
    if (!isTopLeft) {
        origin = MTKTextureLoaderOriginBottomLeft;
    }
    
    MTLTextureUsage usage = MTLTextureUsageShaderRead;
    if (isDebugger) {
        usage |= MTLTextureUsagePixelFormatView;
    }
    
    NSDictionary < MTKTextureLoaderOption, id > *options = @{
        MTKTextureLoaderOptionOrigin: origin,
        MTKTextureLoaderOptionGenerateMipmaps: [NSNumber numberWithBool:FALSE],
        MTKTextureLoaderOptionTextureUsage: [NSNumber numberWithUnsignedLong:usage]
    };
    NSError *error = nil;
    id <MTLTexture> mtlTexture = [_objCObj newTextureWithMDLTexture:irradianceTexture options:options error:&error];
    if (error != nil) {
        NSLog(@"Error: failed to create MTLTexture: %@", error);
    }
    return std::make_shared<Texture>(mtlTexture, *_device);
}

std::array<float, 27> TextureLoader::createSphericalHarmonicsCoefficients(const std::string &path,
                                                                          const std::array<std::string, 6> &imageName) {
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
    
    auto irradianceTexture = [MDLTexture irradianceTextureCubeWithTexture:mdlTexture
                                                                     name:NULL dimensions:simd_make_int2(64, 64) roughness:0];
    
    MDLLightProbe *lightProbe = [[MDLLightProbe alloc] initWithReflectiveTexture:mdlTexture irradianceTexture:irradianceTexture];
    [lightProbe generateSphericalHarmonicsFromIrradiance:2];
    float *coeffs = (float *) lightProbe.sphericalHarmonicsCoefficients.bytes;
    
    std::array<float, 27> result;
    std::copy(coeffs, coeffs + 27, result.data());
    return result;
}

TexturePtr TextureLoader::createBRDFLookupTable() {
    auto brdfFunction = _library.newFunctionWithName("integrateBRDF");
    NSError *error = nil;
    auto brdfPipelineState = [_device->objCObj() newComputePipelineStateWithFunction:brdfFunction->objCObj() error:&error];
    if (error != nil) {
        NSLog(@"Error: failed to create Metal pipeline state: %@", error);
    }
    auto commandBuffer = [_commandQueue.objCObj() commandBuffer];
    auto commandEncoder = [commandBuffer computeCommandEncoder];
    
    const uint32_t size = 256;
    auto descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRG16Float
                                                                         width:size height:size mipmapped:false];
    descriptor.usage = MTLTextureUsageShaderRead | MTLTextureUsageShaderWrite;
    auto lut = [_device->objCObj() newTextureWithDescriptor:descriptor];
    
    [commandEncoder setComputePipelineState:brdfPipelineState];
    [commandEncoder setTexture:lut atIndex:0];
    auto threadsPerThreadgroup = MTLSizeMake(16, 16, 1);
    auto threadgroups = MTLSizeMake(size / threadsPerThreadgroup.width,
                                    size / threadsPerThreadgroup.height, 1);
    [commandEncoder dispatchThreadgroups:threadgroups threadsPerThreadgroup:threadsPerThreadgroup];
    [commandEncoder endEncoding];
    [commandBuffer commit];
    return std::make_shared<Texture>(lut, *_device);
}

TexturePtr TextureLoader::createSpecularTexture(const std::string &path,
                                                   const std::array<std::string, 6> &imageName,
                                                   bool isDebugger, bool isTopLeft) {
    TexturePtr mtlTexture = loadCubeTexture(path, imageName, isTopLeft);
    
    // final texture
    MTLTextureUsage usage = MTLTextureUsageShaderRead;
    if (isDebugger) {
        usage |= MTLTextureUsagePixelFormatView;
    }
    
    MTLTextureDescriptor *descriptor = [[MTLTextureDescriptor alloc] init];
    descriptor.textureType = MTLTextureTypeCube;
    descriptor.pixelFormat = (MTLPixelFormat)mtlTexture->pixelFormat();
    descriptor.width = mtlTexture->width();
    descriptor.height = mtlTexture->height();
    descriptor.mipmapLevelCount = 9;
    descriptor.usage = usage;
    auto specularTexture = [_device->objCObj() newTextureWithDescriptor:descriptor];
    
    // merge
    NSError *error = nil;
    auto function = _library.newFunctionWithName("build_specular");
    auto pipelineState = [_device->objCObj() newComputePipelineStateWithFunction:function->objCObj() error:&error];
    if (error != nil) {
        NSLog(@"Error: failed to create Metal pipeline state: %@", error);
    }
    auto commandBuffer = [_commandQueue.objCObj() commandBuffer];
    
    auto blitEncoder = [commandBuffer blitCommandEncoder];
    [blitEncoder copyFromTexture:mtlTexture->objCObj() sourceSlice:0 sourceLevel:0
                       toTexture:specularTexture destinationSlice:0 destinationLevel:0 sliceCount:6 levelCount:1];
    [blitEncoder endEncoding];
    
    // generate Mipmap
    for (int level = 1; level < 9; level++) {
        std::cout << "Processing level: " << level << std::endl;
        auto commandEncoder = [commandBuffer computeCommandEncoder];
        
        auto size = mtlTexture->width() / int(pow(2, float(level)));
        auto descriptor = [MTLTextureDescriptor textureCubeDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                size:size mipmapped:false];
        descriptor.usage = MTLTextureUsageShaderRead | MTLTextureUsageShaderWrite;
        auto outputTexture = [_device->objCObj() newTextureWithDescriptor:descriptor];
        
        [commandEncoder setComputePipelineState:pipelineState];
        [commandEncoder setTexture:mtlTexture->objCObj() atIndex:0];
        [commandEncoder setTexture:outputTexture atIndex:1];
        
        float roughness = float(level) / 10;
        [commandEncoder setBytes:&roughness length:sizeof(float) atIndex:0];
        auto threadsPerThreadgroup = MTLSizeMake(std::min<size_t>(size, 16), std::min<size_t>(size, 16), 1);
        auto threadgroups = MTLSizeMake(mtlTexture->width() / threadsPerThreadgroup.width,
                                        mtlTexture->width() / threadsPerThreadgroup.height, 6);
        [commandEncoder dispatchThreadgroups:threadgroups threadsPerThreadgroup:threadsPerThreadgroup];
        [commandEncoder endEncoding];
        
        // merge together
        auto blitEncoder = [commandBuffer blitCommandEncoder];
        [blitEncoder copyFromTexture:outputTexture sourceSlice:0 sourceLevel:0
                           toTexture:specularTexture destinationSlice:0 destinationLevel:level sliceCount:6 levelCount:1];
        [blitEncoder endEncoding];
    }
    [commandBuffer commit];
    [commandBuffer waitUntilCompleted];
    
    return std::make_shared<Texture>(specularTexture, *_device);
}

TexturePtr TextureLoader::createMetallicRoughnessTexture(const std::string &path, const std::string &metallic,
                                                            const std::string &roughness, bool isTopLeft) {
    auto metallicTex = loadTexture(path, metallic, isTopLeft);
    auto roughnessTex = loadTexture(path, roughness, isTopLeft);
    
    MTLTextureDescriptor *descriptor = [[MTLTextureDescriptor alloc] init];
    descriptor.textureType = MTLTextureType2D;
    descriptor.pixelFormat = (MTLPixelFormat)metallicTex->pixelFormat();
    descriptor.width = metallicTex->width();
    descriptor.height = metallicTex->height();
    descriptor.usage = (MTLTextureUsage)(metallicTex->usage() | MTL::TextureUsageShaderWrite);
    auto mergedTexture = [_device->objCObj() newTextureWithDescriptor:descriptor];
    
    // merge
    NSError *error = nil;
    auto function = _library.newFunctionWithName("build_metallicRoughness");
    auto pipelineState = [_device->objCObj() newComputePipelineStateWithFunction:function->objCObj() error:&error];
    if (error != nil) {
        NSLog(@"Error: failed to create Metal pipeline state: %@", error);
    }
    auto commandBuffer = [_commandQueue.objCObj() commandBuffer];
    auto commandEncoder = [commandBuffer computeCommandEncoder];
    
    [commandEncoder setComputePipelineState:pipelineState];
    [commandEncoder setTexture:metallicTex->objCObj() atIndex:0];
    [commandEncoder setTexture:roughnessTex->objCObj() atIndex:1];
    [commandEncoder setTexture:mergedTexture atIndex:2];
    
    auto size = metallicTex->width();
    auto threadsPerThreadgroup = MTLSizeMake(std::min<size_t>(size, 16), std::min<size_t>(size, 16), 1);
    auto threadgroups = MTLSizeMake(metallicTex->width() / threadsPerThreadgroup.width,
                                    metallicTex->height() / threadsPerThreadgroup.height, 1);
    [commandEncoder dispatchThreadgroups:threadgroups threadsPerThreadgroup:threadsPerThreadgroup];
    [commandEncoder endEncoding];
    
    [commandBuffer commit];
    [commandBuffer waitUntilCompleted];
    
    return std::make_shared<Texture>(mergedTexture, *_device);
}

//MARK: - 
NSDictionary *textureLoaderDictionary(const TextureLoaderOptions &options) {
    NSDictionary *textureLoaderOptions =
    @{
        MTKTextureLoaderOptionTextureUsage: @(options.usage),
        MTKTextureLoaderOptionTextureStorageMode: @(options.storageMode),
    };
    
    return [textureLoaderOptions copyWithZone:nil];
}

MTL::TexturePtr TextureLoader::makeTexture(const char *name,
                                           float screenScaleFactor,
                                           const TextureLoaderOptions &options,
                                           CFErrorRef *error) {
    NSString *nsname = [[NSString alloc] initWithUTF8String:name];
    NSError *nserror;
    
    id <MTLTexture> objCTexture = [_objCObj newTextureWithName:nsname
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
    
    return std::make_shared<Texture>(objCTexture, *_device);
}

MTL::TexturePtr TextureLoader::makeTexture(const char *URLString,
                                           const TextureLoaderOptions &options,
                                           CFErrorRef *error) {
    NSString *nsURLString = [[NSString alloc] initWithUTF8String:URLString];
    NSURL *URL = [[NSURL alloc] initWithString:nsURLString];
    NSError *nserror;
    
    id <MTLTexture> objCTexture = [_objCObj newTextureWithContentsOfURL:URL
                                                                options:textureLoaderDictionary(options)
                                                                  error:&nserror];
    
    if (!objCTexture) {
        if (error) {
            *error = (__bridge CFErrorRef) nserror;
        }
        return nullptr;
    }
    
    return std::make_shared<Texture>(objCTexture, *_device);
}

}
