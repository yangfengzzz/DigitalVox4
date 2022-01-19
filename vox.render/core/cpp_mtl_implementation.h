//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cpp_mtl_Implementation_hpp
#define cpp_mtl_Implementation_hpp

#include <Availability.h>
#include <CoreFoundation/CoreFoundation.h>

#if __OBJC__
#include <Metal/Metal.h>
#include <MetalKit/MetalKit.h>

#define CPP_METAL_VALIDATE_SIZE( lhs, rhs ) \
    _Static_assert( sizeof( lhs ) == sizeof( rhs ), "mismatching size " # lhs " <> " # rhs "!" )

#define CPP_METAL_CLASS_ALIAS( typename ) \
    namespace cpp_mtl_internal                                   \
    {                                                            \
        using typename = MTL ## typename *;                      \
        CPP_METAL_VALIDATE_SIZE( MTL ## typename *, CFTypeRef ); \
    }

#define CPP_METAL_PROTOCOL_ALIAS( typename ) \
    namespace cpp_mtl_internal                                       \
    {                                                                \
        using typename = id< MTL ## typename >;                      \
        CPP_METAL_VALIDATE_SIZE( id <MTL ## typename >, CFTypeRef ); \
    }

#define CPP_METALKIT_CLASS_ALIAS( typename ) \
    namespace cpp_mtl_internal                                   \
    {                                                            \
        using typename = MTK ## typename *;                      \
        CPP_METAL_VALIDATE_SIZE( MTK ## typename *, CFTypeRef ); \
    }

#define CPP_CoreAnimation_CLASS_ALIAS( typename ) \
    namespace cpp_mtl_internal                                   \
    {                                                            \
        using typename = CA ## typename *;                      \
        CPP_METAL_VALIDATE_SIZE( CA ## typename *, CFTypeRef ); \
    }

namespace cpp_mtl_internal
{
    using ObjCObj = NSObject;
}

#else // if (!__OBJC__)

#include <objc/message.h>


#define CPP_METAL_CLASS_ALIAS( typename ) \
    namespace cpp_mtl_internal            \
    {                                     \
        using typename = id;              \
    }

#define CPP_METAL_PROTOCOL_ALIAS( typename ) \
    namespace cpp_mtl_internal               \
    {                                        \
        using typename = id;                 \
    }

#define CPP_METALKIT_CLASS_ALIAS( typename ) \
    namespace cpp_mtl_internal               \
    {                                        \
        using typename = id;                 \
    }

#define CPP_CoreAnimation_CLASS_ALIAS( typename ) \
    namespace cpp_mtl_internal                    \
    {                                             \
        using typename = id;                      \
    }

namespace cpp_mtl_internal
{
    using ObjCObj = CFTypeRef;
}

#endif // !__OBJC__

#if __OBJC__ && __has_feature(objc_arc)

#define CPP_METAL_PROCESS_LABEL( string, funcname )                                            \
{                                                                                              \
    CFStringRef cfString = CFStringCreateWithCString(nullptr, string, kCFStringEncodingASCII); \
    funcname(cfString);                                                                        \
}

#else

#define CPP_METAL_PROCESS_LABEL( string, funcname )                                            \
{                                                                                              \
    CFStringRef cfString = CFStringCreateWithCString(nullptr, string, kCFStringEncodingASCII); \
    funcname(cfString);                                                                        \
    CFRelease(cfString);                                                                       \
}

#endif

CPP_METAL_PROTOCOL_ALIAS( Buffer );
CPP_METAL_PROTOCOL_ALIAS( CommandBuffer );
CPP_METAL_PROTOCOL_ALIAS( CommandQueue );
CPP_METAL_PROTOCOL_ALIAS( CommandEncoder );
CPP_METAL_PROTOCOL_ALIAS( DepthStencilState );
CPP_METAL_PROTOCOL_ALIAS( Device );
CPP_METAL_PROTOCOL_ALIAS( Drawable );
CPP_METAL_PROTOCOL_ALIAS( Library );
CPP_METAL_PROTOCOL_ALIAS( Function );
CPP_METAL_PROTOCOL_ALIAS( RenderCommandEncoder );
CPP_METAL_PROTOCOL_ALIAS( RenderPipelineState );
CPP_METAL_PROTOCOL_ALIAS( Resource );
CPP_METAL_PROTOCOL_ALIAS( Texture );

CPP_METAL_CLASS_ALIAS( StencilDescriptor );
CPP_METAL_CLASS_ALIAS( DepthStencilDescriptor );
CPP_METAL_CLASS_ALIAS( RenderPassAttachmentDescriptor );
CPP_METAL_CLASS_ALIAS( RenderPassDepthAttachmentDescriptor );
CPP_METAL_CLASS_ALIAS( RenderPassStencilAttachmentDescriptor );
CPP_METAL_CLASS_ALIAS( RenderPassColorAttachmentDescriptor );
CPP_METAL_CLASS_ALIAS( RenderPassColorAttachmentDescriptorArray );
CPP_METAL_CLASS_ALIAS( RenderPassDescriptor );
CPP_METAL_CLASS_ALIAS( RenderPipelineDescriptor );
CPP_METAL_CLASS_ALIAS( RenderPipelineColorAttachmentDescriptor );
CPP_METAL_CLASS_ALIAS( RenderPipelineColorAttachmentDescriptorArray );
CPP_METAL_CLASS_ALIAS( TextureDescriptor );
CPP_METAL_CLASS_ALIAS( VertexBufferLayoutDescriptor );
CPP_METAL_CLASS_ALIAS( VertexAttributeDescriptor );
CPP_METAL_CLASS_ALIAS( VertexBufferLayoutDescriptorArray );
CPP_METAL_CLASS_ALIAS( VertexAttributeDescriptorArray );
CPP_METAL_CLASS_ALIAS( VertexDescriptor );
CPP_METAL_CLASS_ALIAS( FunctionConstantValues );
CPP_METAL_CLASS_ALIAS( RenderPipelineReflection );

CPP_METALKIT_CLASS_ALIAS( TextureLoader );
CPP_CoreAnimation_CLASS_ALIAS(MetalLayer);

#define CPP_METAL_MOVE_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION( classname ) \
    inline classname::classname(classname && rhs)                                   \
    : m_objCObj(rhs.m_objCObj)                                                      \
    , m_device(rhs.m_device)                                                        \
    {                                                                               \
        rhs.m_objCObj = nullptr;                                                    \
        rhs.m_device = nullptr;                                                     \
    }                                                                               \
                                                                                    \
    inline classname & classname::operator=(classname && rhs)                       \
    {                                                                               \
        m_objCObj = rhs.m_objCObj;                                                  \
        m_device = rhs.m_device;                                                    \
        rhs.m_objCObj = nullptr;                                                    \
        rhs.m_device = nullptr;                                                     \
                                                                                    \
        return *this;                                                               \
    }

#define CPP_METAL_OBJCOBJ_GETTER_IMPLEMENATATION( classname )       \
    inline cpp_mtl_internal::classname classname::objCObj() const \
    {                                                             \
        return m_objCObj;                                         \
    }

#define CPP_METAL_DECLARE_FUNCTION_POINTER( methodName ) \
    methodName ## Type methodName

#define CPP_METAL_VIRTUAL virtual

namespace cpp_mtl_internal
{
class DeviceInternals;
}

#endif // cpp_mtl_Implementation_hpp
