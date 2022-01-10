/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for C++ Metal device class wrapper
*/

#ifndef CPPMetalDevice_hpp
#define CPPMetalDevice_hpp
#include <memory>
#include "CPPMetalImplementation.hpp"
#include "CPPMetalTypes.hpp"
#include "CPPMetalResourceEnum.hpp"
#include "CPPMetalAllocator.hpp"


namespace MTL
{


class Device;
class Allocator;

Device *CreateSystemDefaultDevice(Allocator *allocator = nullptr);

class Texture;
class TextureDescriptor;
class Buffer;
class Library;
class RenderPipelineState;
class RenderPipelineDescriptor;
class DepthStencilState;
class DepthStencilDescriptor;
class CommandQueue;
class Resource;


typedef enum GPUFamily {
    GPUFamilyApple1 = 1001,
    GPUFamilyApple2 = 1002,
    GPUFamilyApple3 = 1003,
    GPUFamilyApple4 = 1004,
    GPUFamilyApple5 = 1005,
    GPUFamilyApple6 = 1006,
    GPUFamilyApple7 = 1007,

    GPUFamilyMac1 = 2001,
    GPUFamilyMac2 = 2002,

    GPUFamilyCommon1 = 3001,
    GPUFamilyCommon2 = 3002,
    GPUFamilyCommon3 = 3003,

    GPUFamilyMacCatalyst1 = 4001,
    GPUFamilyMacCatalyst2 = 4002,
} GPUFamily;


class Device
{
public:

    Device();

    Device(const Device & rhs);

    Device(Device && rhs);

    Device & operator=(const Device & rhs);

    Device & operator=(Device && rhs);

    CPP_METAL_VIRTUAL ~Device();

    bool operator==(const Device & rhs) const;

    Allocator & allocator();

    Library *newDefaultLibrary();
    Library makeDefaultLibrary();

    Library *newLibraryWithFile(const char* filepath, CFErrorRef *error = nullptr);
    Library *newLibraryWithURL(const CFURLRef URL, CFErrorRef *error = nullptr);
    Library makeLibrary(const char* filepath, CFErrorRef *error = nullptr);
    Library makeLibrary(const CFURLRef URL, CFErrorRef *error = nullptr);

    RenderPipelineState *newRenderPipelineStateWithDescriptor(const RenderPipelineDescriptor & descriptor,
                                                              CFErrorRef *error = nullptr);
    RenderPipelineState makeRenderPipelineState(const RenderPipelineDescriptor & descriptor,
                                                CFErrorRef *error = nullptr);

    DepthStencilState *newDepthStencilStateWithDescriptor(const DepthStencilDescriptor& descriptor);
    DepthStencilState makeDepthStencilState(const DepthStencilDescriptor& descriptor);

    CommandQueue *newCommandQueue();
    CommandQueue makeCommandQueue();

    Buffer *newBufferWithLength(UInteger length, ResourceOptions options = ResourceOptionsDefault);
    Buffer makeBuffer(UInteger length, ResourceOptions options = ResourceOptionsDefault);

    Buffer *newBufferWithBytes(const void *bytes, UInteger length, ResourceOptions options = ResourceOptionsDefault);
    Buffer makeBuffer(const void *bytes, UInteger length, ResourceOptions options = ResourceOptionsDefault);

    Texture *newTextureWithDescriptor(const TextureDescriptor & descriptor);
    Texture makeTexture(const TextureDescriptor & descriptor);

    bool supportsFamily(GPUFamily family) const;

    const char *name() const;

private:

    CPPMetalInternal::Device m_objCObj;

    std::shared_ptr<CPPMetalInternal::DeviceInternals> m_internals;

    Allocator *m_allocator;

public: // Public methods for CPPMetal internal implementation

    Device(CPPMetalInternal::Device objCObj, Allocator *allocator);

    CPPMetalInternal::Device objCObj() const;

    CPPMetalInternal::DeviceInternals & internals();

};


//=================================================
#pragma mark - Device inline method implementations

CPP_METAL_OBJCOBJ_GETTER_IMPLEMENATATION(Device);

inline Device::Device(Device && rhs)
: m_objCObj(rhs.m_objCObj)
, m_internals(rhs.m_internals)
, m_allocator(rhs.m_allocator)
{
    rhs.m_objCObj = nullptr;
    rhs.m_internals = nullptr;
}

inline Device & Device::operator=(Device && rhs)
{
    m_objCObj = rhs.m_objCObj;
    m_internals = rhs.m_internals;
    m_allocator = rhs.m_allocator;
    rhs.m_objCObj = nullptr;
    rhs.m_internals = nullptr;

    return *this;
}

inline CPPMetalInternal::DeviceInternals & Device::internals()
{
    return *m_internals;
}

inline Allocator & Device::allocator()
{
    return *m_allocator;
}

} // namespace MTL

#endif // CPPMetalDevice_hpp
