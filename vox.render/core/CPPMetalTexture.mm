/*
 See LICENSE folder for this sample’s licensing information.
 
 Abstract:
 Implementation of C++ Metal texture class wrappers
 */


#include "CPPMetalTexture.hpp"
#include "CPPMetalInternalMacros.h"
#include "CPPMetalDevice.hpp"

using namespace MTL;

#pragma mark - Enum Validation

CPP_METAL_VALIDATE_ENUM_ALIAS(TextureType1D);
CPP_METAL_VALIDATE_ENUM_ALIAS(TextureType1DArray);
CPP_METAL_VALIDATE_ENUM_ALIAS(TextureType2D);
CPP_METAL_VALIDATE_ENUM_ALIAS(TextureType2DMultisample);
CPP_METAL_VALIDATE_ENUM_ALIAS(TextureTypeCube);
CPP_METAL_VALIDATE_ENUM_ALIAS(TextureTypeCubeArray);
CPP_METAL_VALIDATE_ENUM_ALIAS(TextureType3D);
#if TARGET_OS_OSX
CPP_METAL_VALIDATE_ENUM_ALIAS(TextureType2DMultisampleArray);
#endif
CPP_METAL_VALIDATE_ENUM_ALIAS(TextureTypeTextureBuffer);

CPP_METAL_VALIDATE_ENUM_ALIAS(TextureUsageUnknown);
CPP_METAL_VALIDATE_ENUM_ALIAS(TextureUsageShaderRead);
CPP_METAL_VALIDATE_ENUM_ALIAS(TextureUsageShaderWrite);
CPP_METAL_VALIDATE_ENUM_ALIAS(TextureUsageRenderTarget);
CPP_METAL_VALIDATE_ENUM_ALIAS(TextureUsagePixelFormatView);

#pragma mark - TextureDescriptor

TextureDescriptor::TextureDescriptor() :
m_objCObj([MTLTextureDescriptor new]) {
    // Member initialization only
}


TextureDescriptor::TextureDescriptor(const TextureDescriptor &rhs) :
m_objCObj(rhs.m_objCObj) {
    // Member initialization only
}


TextureDescriptor &TextureDescriptor::operator=(const TextureDescriptor &rhs) {
    m_objCObj = rhs.m_objCObj;
    
    return *this;
}

TextureDescriptor::~TextureDescriptor() {
    m_objCObj = nil;
}

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(TextureDescriptor, TextureType, textureType);

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(TextureDescriptor, PixelFormat, pixelFormat);

CPP_METAL_READWRITE_PROPERTY_IMPLEMENTATION(TextureDescriptor, UInteger, width);

CPP_METAL_READWRITE_PROPERTY_IMPLEMENTATION(TextureDescriptor, UInteger, height);

CPP_METAL_READWRITE_PROPERTY_IMPLEMENTATION(TextureDescriptor, UInteger, depth);

CPP_METAL_READWRITE_PROPERTY_IMPLEMENTATION(TextureDescriptor, UInteger, mipmapLevelCount);

CPP_METAL_READWRITE_PROPERTY_IMPLEMENTATION(TextureDescriptor, UInteger, arrayLength);

CPP_METAL_READWRITE_PROPERTY_IMPLEMENTATION(TextureDescriptor, UInteger, sampleCount);

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(TextureDescriptor, CPUCacheMode, cpuCacheMode);

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(TextureDescriptor, ResourceOptions, resourceOptions);

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(TextureDescriptor, StorageMode, storageMode);

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(TextureDescriptor, TextureUsage, usage);

void TextureDescriptor::usage(UInteger value) {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    m_objCObj.usage = value;
}

#pragma mark - Texture

Texture::Texture(CPPMetalInternal::Texture objCObj, Device &device)
: Resource(objCObj, device) {
    // Member initialization only
}

Texture::Texture(const Texture &rhs)
: Resource(rhs) {
    // Member initialization only
}

Texture &Texture::operator=(const Texture &rhs) {
    Resource::operator=(rhs);
    
    return *this;
}

Texture::~Texture() {
}

TextureType Texture::textureType() const {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    return (TextureType) (((id <MTLTexture>) m_objCObj).textureType);
}

PixelFormat Texture::pixelFormat() const {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    return (PixelFormat) (((id <MTLTexture>) m_objCObj).pixelFormat);
}

UInteger Texture::width() const {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    return ((id <MTLTexture>) m_objCObj).width;
}

UInteger Texture::height() const {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    return ((id <MTLTexture>) m_objCObj).height;
}

UInteger Texture::depth() const {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    return ((id <MTLTexture>) m_objCObj).depth;
}

UInteger Texture::mipmapLevelCount() const {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    return ((id <MTLTexture>) m_objCObj).mipmapLevelCount;
}

UInteger Texture::arrayLength() const {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    return ((id <MTLTexture>) m_objCObj).arrayLength;
}

UInteger Texture::sampleCount() const {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    return ((id <MTLTexture>) m_objCObj).sampleCount;
}

TextureUsage Texture::usage() const {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    return (TextureUsage) (((id <MTLTexture>) m_objCObj).usage);
}

void Texture::replaceRegion(const Region &region,
                            UInteger mipmapLevel,
                            UInteger slice,
                            const void *pixelBytes,
                            UInteger bytesPerRow,
                            UInteger bytesPerImage) {
    const MTLRegion *mtlRegion = (const MTLRegion *) &region;
    [((id <MTLTexture>) m_objCObj) replaceRegion:*mtlRegion
                                     mipmapLevel:mipmapLevel
                                           slice:slice
                                       withBytes:pixelBytes
                                     bytesPerRow:bytesPerRow
                                   bytesPerImage:bytesPerImage];
}

void Texture::getBytes(void *pixelBytes,
                       UInteger bytesPerRow,
                       UInteger bytesPerImage,
                       const Region &sourceRegion,
                       UInteger mipmapLevel,
                       UInteger slice) {
    const MTLRegion *mtlRegion = (const MTLRegion *) &sourceRegion;
    [((id <MTLTexture>) m_objCObj) getBytes:pixelBytes
                                bytesPerRow:bytesPerRow
                              bytesPerImage:bytesPerImage
                                 fromRegion:*mtlRegion
                                mipmapLevel:mipmapLevel
                                      slice:slice];
    
}

void Texture::replaceRegion(const Region &region,
                            UInteger mipmapLevel,
                            const void *pixelBytes,
                            UInteger bytesPerRow) {
    const MTLRegion *mtlRegion = (const MTLRegion *) &region;
    [((id <MTLTexture>) m_objCObj) replaceRegion:*mtlRegion
                                     mipmapLevel:mipmapLevel
                                       withBytes:pixelBytes
                                     bytesPerRow:bytesPerRow];
}

void Texture::getBytes(void *pixelBytes,
                       UInteger bytesPerRow,
                       const Region &sourceRegion,
                       UInteger mipmapLevel) {
    const MTLRegion *mtlRegion = (const MTLRegion *) &sourceRegion;
    [((id <MTLTexture>) m_objCObj) getBytes:pixelBytes
                                bytesPerRow:bytesPerRow
                                 fromRegion:*mtlRegion
                                mipmapLevel:mipmapLevel];
}

CPP_METAL_DEVICE_GETTER_IMPLEMENTATION(Texture);
