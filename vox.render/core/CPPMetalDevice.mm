/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Implementation of C++ Metal device class wrapper
*/
#include <TargetConditionals.h>

#include "CPPMetalDevice.hpp"
#include "CPPMetalBuffer.hpp"
#include "CPPMetalCommandQueue.hpp"
#include "CPPMetalDepthStencil.hpp"
#include "CPPMetalLibrary.hpp"
#include "CPPMetalRenderPipeline.hpp"
#include "CPPMetalTexture.hpp"


#include "CPPMetalDeviceInternals.h"
#include "CPPMetalInternalMacros.h"


using namespace MTL;

namespace MTL
{

// Global Default Allocator
static Allocator DefaultAllocator;

Device *CreateSystemDefaultDevice(Allocator *allocator)
{
    id<MTLDevice> objCObj = MTLCreateSystemDefaultDevice();

    if(!objCObj)
    {
        return nullptr;
    }

    if(!allocator)
    {
        allocator = &DefaultAllocator;
    }

    Device *device = allocator->construct<Device>(objCObj, allocator);

    return device;
}

}

Device::Device(CPPMetalInternal::Device objCObj, Allocator *allocator)
: m_objCObj(objCObj)
, m_internals()
, m_allocator(allocator)
{
    auto deleter = [allocator](CPPMetalInternal::DeviceInternals *ptr){ allocator->destroy(ptr); };

    CPPMetalInternal::DeviceInternals *internals =
        allocator->construct<CPPMetalInternal::DeviceInternals>(allocator);

    // Set up shared_ptr with deleter
    std::shared_ptr<CPPMetalInternal::DeviceInternals> shared_internals(internals, deleter);

    m_internals = shared_internals;
}

Device::Device()
: m_objCObj(nil)
, m_internals(nullptr)
, m_allocator(nullptr)
{

}

Device::Device(const Device & rhs)
: m_objCObj(rhs.m_objCObj)
, m_internals(rhs.m_internals)
, m_allocator(rhs.m_allocator)
{
}

Device & Device::operator=(const Device & rhs)
{
    m_objCObj = rhs.m_objCObj;
    m_internals = rhs.m_internals;
    m_allocator = rhs.m_allocator;
    return *this;
}

Device::~Device()
{
    m_objCObj = nil;
    m_internals = nullptr;
    m_allocator = nullptr;
}

bool Device::operator==(const Device & rhs) const
{
    return [this->m_objCObj isEqual:rhs.m_objCObj];
}

Library *Device::newDefaultLibrary()
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();

    id<MTLLibrary> objCObj = [m_objCObj newDefaultLibrary];

    Library *library = construct<Library>(allocator(), objCObj, *this);

    return library;
}

Library Device::makeDefaultLibrary()
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();

    id<MTLLibrary> objCObj = [m_objCObj newDefaultLibrary];

    return Library(objCObj, *this);
}

Library *Device::newLibraryWithFile(const char* filepath, CFErrorRef *error)
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();

    NSString *nsFilepath = [[NSString alloc] initWithUTF8String:filepath];

    NSError *nserror;
    id<MTLLibrary> objCObj = [m_objCObj newLibraryWithFile:nsFilepath
                                                     error:&nserror];

    if(!objCObj)
    {
        if(error)
        {
            *error = (__bridge CFErrorRef)nserror;
        }
        return nullptr;
    }

    Library *library = construct<Library>(allocator(), objCObj, *this);

    return library;
}

Library *Device::newLibraryWithURL(const CFURLRef URL, CFErrorRef *error)
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();

    NSError *nserror;
    id<MTLLibrary> objCObj = [m_objCObj newLibraryWithURL:(__bridge NSURL*)URL
                                                    error:&nserror];
    if(!objCObj)
    {
        if(error)
        {
            *error = (__bridge CFErrorRef)nserror;
        }
        return nullptr;
    }

    Library *library = construct<Library>(allocator(), objCObj, *this);

    return library;
}


Library Device::makeLibrary(const char* filepath, CFErrorRef *error)
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();

    NSString *nsFilepath = [[NSString alloc] initWithUTF8String:filepath];

    NSError *nserror;
    id<MTLLibrary> objCObj = [m_objCObj newLibraryWithFile:nsFilepath
                                                     error:&nserror];

    if(!objCObj)
    {
        if(error)
        {
            *error = (__bridge CFErrorRef)nserror;
        }
    }

    return Library(objCObj, *this);
}

Library Device::makeLibrary(const CFURLRef URL, CFErrorRef *error)
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();

    NSError *nserror;
    id<MTLLibrary> objCObj = [m_objCObj newLibraryWithURL:(__bridge NSURL*)URL
                                                     error:&nserror];

    if(!objCObj)
    {
        if(error)
        {
            *error = (__bridge CFErrorRef)nserror;
        }
    }

    return Library(objCObj, *this);
}

RenderPipelineState *Device::newRenderPipelineStateWithDescriptor(const RenderPipelineDescriptor & descriptor,
                                                                  CFErrorRef *error)
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();

    NSError *nserror;
    const id<MTLRenderPipelineState> objCObj = [m_objCObj newRenderPipelineStateWithDescriptor:descriptor.objCObj()
                                                                                         error:&nserror];
    if(!objCObj)
    {
        if(error)
        {
            *error = (__bridge CFErrorRef)nserror;
        }
        return nullptr;
    }

    RenderPipelineState *pipeline = construct<RenderPipelineState>(allocator(), objCObj, *this);

    return pipeline;
}

RenderPipelineState Device::makeRenderPipelineState(const RenderPipelineDescriptor & descriptor,
                                                    CFErrorRef *error)
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();

    NSError *nserror;
    const id<MTLRenderPipelineState> objCObj = [m_objCObj newRenderPipelineStateWithDescriptor:descriptor.objCObj()
                                                                                         error:&nserror];
    if(!objCObj)
    {
        if(error)
        {
            *error = (__bridge CFErrorRef)nserror;
        }
    }

    return RenderPipelineState(objCObj, *this);
}

DepthStencilState *Device::newDepthStencilStateWithDescriptor(const DepthStencilDescriptor & descriptor)
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();

    const id<MTLDepthStencilState> objCObj = [m_objCObj newDepthStencilStateWithDescriptor:descriptor.objCObj()];

    if(!objCObj)
    {
        return nullptr;
    }

    DepthStencilState *state = construct<DepthStencilState>(allocator(), objCObj, *this);

    return state;
}

DepthStencilState Device::makeDepthStencilState(const DepthStencilDescriptor & descriptor)
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();

    const id<MTLDepthStencilState> objCObj = [m_objCObj newDepthStencilStateWithDescriptor:descriptor.objCObj()];

    return DepthStencilState(objCObj, *this);
}

CommandQueue *Device::newCommandQueue()
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();

    id<MTLCommandQueue> objCObj = [m_objCObj newCommandQueue];

    if(!objCObj)
    {
        return nullptr;
    }

    CommandQueue *commandQueue = construct<CommandQueue>(allocator(), objCObj, *this);

    return commandQueue;
}

CommandQueue Device::makeCommandQueue()
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();

    id<MTLCommandQueue> objCObj = [m_objCObj newCommandQueue];

    return CommandQueue(objCObj, *this);
}

Buffer *Device::newBufferWithLength(UInteger length, ResourceOptions options)
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();

    const id<MTLBuffer> objCObj = [m_objCObj newBufferWithLength:length
                                                         options:(MTLResourceOptions)options];

    if(!objCObj)
    {
        return nullptr;
    }

    Buffer *buffer = construct<Buffer>(allocator(), objCObj, *this);

    return buffer;
}

Buffer Device::makeBuffer(UInteger length, ResourceOptions options)
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();

    const id<MTLBuffer> objCObj = [m_objCObj newBufferWithLength:length
                                                         options:(MTLResourceOptions)options];

    return Buffer(objCObj, *this);
}

Buffer *Device::newBufferWithBytes(const void *bytes, UInteger length, ResourceOptions options)
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();

    const id<MTLBuffer> objCObj = [m_objCObj newBufferWithBytes:bytes
                                                         length:length
                                                        options:(MTLResourceOptions)options];

    if(!objCObj)
    {
        return nullptr;
    }

    Buffer *buffer = construct<Buffer>(allocator(), objCObj, *this);

    return buffer;
}

Buffer Device::makeBuffer(const void *bytes, UInteger length, ResourceOptions options)
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();

    const id<MTLBuffer> objCObj = [m_objCObj newBufferWithBytes:bytes
                                                         length:length
                                                        options:(MTLResourceOptions)options];

    Buffer buffer(objCObj, *this);

    return buffer;
}

Texture *Device::newTextureWithDescriptor(const TextureDescriptor & descriptor)
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();

    const id<MTLTexture> objCObj = [m_objCObj newTextureWithDescriptor:descriptor.objCObj()];

    if(!objCObj)
    {
        return nullptr;
    }

    Texture *texture = construct<Texture>(allocator(), objCObj, *this);

    return texture;
}

Texture Device::makeTexture(const TextureDescriptor & descriptor)
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();

    const id<MTLTexture> objCObj = [m_objCObj newTextureWithDescriptor:descriptor.objCObj()];

    return Texture(objCObj, *this);
}

const char* Device::name() const
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();

    return m_objCObj.name.UTF8String;
}

bool Device::supportsFamily(GPUFamily family) const
{
    return [m_objCObj supportsFamily:(MTLGPUFamily)family];
}

CPP_METAL_VALIDATE_ENUM_ALIAS( GPUFamilyApple1 );
CPP_METAL_VALIDATE_ENUM_ALIAS( GPUFamilyApple2 );
CPP_METAL_VALIDATE_ENUM_ALIAS( GPUFamilyApple3 );
CPP_METAL_VALIDATE_ENUM_ALIAS( GPUFamilyApple4 );
CPP_METAL_VALIDATE_ENUM_ALIAS( GPUFamilyApple5 );

#if !TARGET_OS_TV
CPP_METAL_VALIDATE_ENUM_ALIAS( GPUFamilyApple6 );
CPP_METAL_VALIDATE_ENUM_ALIAS( GPUFamilyApple7 );
#endif

CPP_METAL_VALIDATE_ENUM_ALIAS( GPUFamilyMac1 );
CPP_METAL_VALIDATE_ENUM_ALIAS( GPUFamilyMac2 );
CPP_METAL_VALIDATE_ENUM_ALIAS( GPUFamilyCommon1 );
CPP_METAL_VALIDATE_ENUM_ALIAS( GPUFamilyCommon2 );
CPP_METAL_VALIDATE_ENUM_ALIAS( GPUFamilyCommon3 );
CPP_METAL_VALIDATE_ENUM_ALIAS( GPUFamilyMacCatalyst1 );
CPP_METAL_VALIDATE_ENUM_ALIAS( GPUFamilyMacCatalyst2 );
