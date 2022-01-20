//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cpp_mtl_Device_hpp
#define cpp_mtl_Device_hpp

#include <memory>
#include "cpp_mtl_implementation.h"
#include "cpp_mtl_types.h"
#include "cpp_mtl_resource_enum.h"
#include "cpp_mtl_allocator.h"

namespace MTL {
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


class Device {
public:
    Device();
    
    Device(Device &&rhs);
    
    CPP_METAL_VIRTUAL ~Device();
    
    bool operator==(const Device &rhs) const;
    
    Allocator &allocator();
    
    Library *newDefaultLibrary();
    
    Library makeDefaultLibrary();
    
    Library *newLibraryWithFile(const char *filepath, CFErrorRef *error = nullptr);
    
    Library *newLibraryWithURL(const CFURLRef URL, CFErrorRef *error = nullptr);
    
    Library makeLibrary(const char *filepath, CFErrorRef *error = nullptr);
    
    Library makeLibrary(const CFURLRef URL, CFErrorRef *error = nullptr);
    
    RenderPipelineState *newRenderPipelineStateWithDescriptor(const RenderPipelineDescriptor &descriptor);
    
    RenderPipelineState makeRenderPipelineState(const RenderPipelineDescriptor &descriptor);
    
    DepthStencilState *newDepthStencilStateWithDescriptor(const DepthStencilDescriptor &descriptor);
    
    DepthStencilState makeDepthStencilState(const DepthStencilDescriptor &descriptor);
    
    CommandQueue *newCommandQueue();
    
    CommandQueue makeCommandQueue();
    
    Buffer *newBufferWithLength(UInteger length, ResourceOptions options = ResourceOptionsDefault);
    
    Buffer makeBuffer(UInteger length, ResourceOptions options = ResourceOptionsDefault);
    
    Buffer *newBufferWithBytes(const void *bytes, UInteger length, ResourceOptions options = ResourceOptionsDefault);
    
    Buffer makeBuffer(const void *bytes, UInteger length, ResourceOptions options = ResourceOptionsDefault);
    
    Texture *newTextureWithDescriptor(const TextureDescriptor &descriptor);
    
    Texture makeTexture(const TextureDescriptor &descriptor);
    
    bool supportsFamily(GPUFamily family) const;
    
    const char *name() const;
        
private:
    cpp_mtl_internal::Device m_objCObj;
    
    std::shared_ptr<cpp_mtl_internal::DeviceInternals> m_internals;
    
    Allocator *m_allocator;
    
public: // Public methods for cpp_mtl_ internal implementation
    Device(cpp_mtl_internal::Device objCObj, Allocator *allocator);
    
    cpp_mtl_internal::Device objCObj() const;
    
    cpp_mtl_internal::DeviceInternals &internals();    
};


#pragma mark - Device inline method implementations

CPP_METAL_OBJCOBJ_GETTER_IMPLEMENATATION(Device);

inline cpp_mtl_internal::DeviceInternals &Device::internals() {
    return *m_internals;
}

inline Allocator &Device::allocator() {
    return *m_allocator;
}

} // namespace MTL

#endif // cpp_mtl_Device_hpp
