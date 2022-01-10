/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for C++ Metal texture class wrappers
*/

#ifndef CPPMetalTexture_hpp
#define CPPMetalTexture_hpp

#include "CPPMetalPixelFormat.hpp"
#include "CPPMetalResource.hpp"
#include "CPPMetalTypes.hpp"


namespace MTL
{


typedef enum TextureType
{
    TextureType1D = 0,
    TextureType1DArray = 1,
    TextureType2D = 2,
    TextureType2DArray = 3,
    TextureType2DMultisample = 4,
    TextureTypeCube = 5,
    TextureTypeCubeArray API_AVAILABLE(macos(10.11), ios(11.0)) = 6,
    TextureType3D = 7,
    TextureType2DMultisampleArray API_AVAILABLE(macos(10.14)) API_UNAVAILABLE(ios) = 8,
    TextureTypeTextureBuffer API_AVAILABLE(macos(10.14), ios(12.0)) = 9
} TextureType;


typedef enum TextureUsage
{
    TextureUsageUnknown         = 0x0000,
    TextureUsageShaderRead      = 0x0001,
    TextureUsageShaderWrite     = 0x0002,
    TextureUsageRenderTarget    = 0x0004,
    TextureUsagePixelFormatView = 0x0010,
} TextureUsage API_AVAILABLE(macos(10.11), ios(9.0));

struct TextureDescriptor
{
public:

    TextureDescriptor();

    TextureDescriptor(const TextureDescriptor & rhs);

    TextureDescriptor & operator=(const TextureDescriptor & rhs);

    CPP_METAL_VIRTUAL ~TextureDescriptor();

    TextureType textureType() const;
    void        textureType(TextureType type);

    PixelFormat pixelFormat() const;
    void        pixelFormat(PixelFormat type);

    UInteger width() const;
    void     width(UInteger value);

    UInteger height() const;
    void     height(UInteger value);

    UInteger depth() const;
    void     depth(UInteger value);

    UInteger mipmapLevelCount() const;
    void     mipmapLevelCount(UInteger value);

    UInteger arrayLength() const;
    void     arrayLength(UInteger value);

    UInteger sampleCount() const;
    void     sampleCount(UInteger value);

    CPUCacheMode cpuCacheMode() const;
    void         cpuCacheMode(CPUCacheMode mode);

    ResourceOptions resourceOptions() const;
    void            resourceOptions(ResourceOptions options);

    StorageMode storageMode() const;
    void        storageMode(StorageMode mode);

    TextureUsage usage() const;
    void         usage(TextureUsage value);
    void         usage(UInteger value);

private:

    CPPMetalInternal::TextureDescriptor m_objCObj;

public: // Public methods for CPPMetal internal implementation

    CPPMetalInternal::TextureDescriptor objCObj() const;

};

class Texture : public Resource
{
public:

    Texture();

    Texture(const Texture & rhs);

    Texture(Texture && rhs);

    Texture & operator=(const Texture & rhs);

    Texture & operator=(Texture && rhs);

    CPP_METAL_VIRTUAL ~Texture();

    TextureType textureType() const;

    UInteger width() const;

    UInteger height() const;

    UInteger depth() const;

    UInteger mipmapLevelCount() const;

    UInteger arrayLength() const;

    UInteger sampleCount() const;

    TextureUsage usage() const;

    void replaceRegion(const Region & region,
                       UInteger mipmapLevel,
                       UInteger slice,
                       const void *pixelBytes,
                       UInteger bytesPerRow,
                       UInteger bytesPerImage);

    void getBytes(void * pixelBytes,
                  UInteger bytesPerRow,
                  UInteger bytesPerImage,
                  const Region & sourceRegion,
                  UInteger mipmapLevel,
                  UInteger slice);

    void replaceRegion(const Region & region,
                       UInteger mipmapLevel,
                       const void *pixelBytes,
                       UInteger bytesPerRow);

    void getBytes(void * pixelBytes,
                  UInteger bytesPerRow,
                  const Region & sourceRegion,
                  UInteger mipmapLevel);

    Device device() const;

public: // Public methods for CPPMetal internal implementation

    Texture(CPPMetalInternal::Texture objCObj, Device & device);

    CPPMetalInternal::Texture objCObj() const;

};

//============================================================
#pragma mark - TextureDescriptor inline method implementations

inline CPPMetalInternal::TextureDescriptor TextureDescriptor::objCObj() const
{
    return m_objCObj;
}


//==================================================
#pragma mark - Texture inline method implementations

inline Texture::Texture()
{
    // Member initialization only
}

inline Texture::Texture(Texture && rhs)
: Resource(rhs)
{
    // Member initialization only
}

inline Texture & Texture::operator=(Texture && rhs)
{
    Resource::operator=(rhs);

    return *this;
}

inline CPPMetalInternal::Texture Texture::objCObj() const
{
    return (CPPMetalInternal::Texture)m_objCObj;;
}


} // namespace MTL

#endif // CPPMetalTexture_hpp
