/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Piece of the CPPMetal wrapper implementation on which public classes have dependancies.  Code here
 is not intended to be part of the CPPMetal Interface or used outside of CPPMetal Library clases.
*/

#ifndef CPPMetalImplementation_hpp
#define CPPMetalImplementation_hpp

#include <Availability.h>
#include <CoreFoundation/CoreFoundation.h>

#if __OBJC__
#include <Metal/Metal.h>
#include <MetalKit/MetalKit.h>

#define CPP_METAL_VALIDATE_SIZE( lhs, rhs ) \
    _Static_assert( sizeof( lhs ) == sizeof( rhs ), "mismatching size " # lhs " <> " # rhs "!" )

#define CPP_METAL_CLASS_ALIAS( typename ) \
    namespace CPPMetalInternal                                   \
    {                                                            \
        using typename = MTL ## typename *;                      \
        CPP_METAL_VALIDATE_SIZE( MTL ## typename *, CFTypeRef ); \
    }

#define CPP_METAL_PROTOCOL_ALIAS( typename ) \
    namespace CPPMetalInternal                                       \
    {                                                                \
        using typename = id< MTL ## typename >;                      \
        CPP_METAL_VALIDATE_SIZE( id <MTL ## typename >, CFTypeRef ); \
    }

#define CPP_METALKIT_CLASS_ALIAS( typename ) \
    namespace CPPMetalInternal                                   \
    {                                                            \
        using typename = MTK ## typename *;                      \
        CPP_METAL_VALIDATE_SIZE( MTK ## typename *, CFTypeRef ); \
    }

namespace CPPMetalInternal
{
    using ObjCObj = NSObject;
}

#else // if (!__OBJC__)

#include <objc/message.h>


#define CPP_METAL_CLASS_ALIAS( typename ) \
    namespace CPPMetalInternal            \
    {                                     \
        using typename = id;              \
    }

#define CPP_METAL_PROTOCOL_ALIAS( typename ) \
    namespace CPPMetalInternal               \
    {                                        \
        using typename = id;                 \
    }

#define CPP_METALKIT_CLASS_ALIAS( typename ) \
    namespace CPPMetalInternal               \
    {                                        \
        using typename = id;                 \
    }

namespace CPPMetalInternal
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

CPP_METALKIT_CLASS_ALIAS( View );
CPP_METALKIT_CLASS_ALIAS( TextureLoader );

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
    inline CPPMetalInternal::classname classname::objCObj() const \
    {                                                             \
        return m_objCObj;                                         \
    }

#define CPP_METAL_DECLARE_FUNCTION_POINTER( methodName ) \
    methodName ## Type methodName

#define CPP_METAL_VIRTUAL virtual

namespace CPPMetalInternal
{
class DeviceInternals;
}

#endif // CPPMetalImplementation_hpp
