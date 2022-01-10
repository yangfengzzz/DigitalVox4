/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for C++ Metal render state pipeline class wrappers
*/

#ifndef CPPMetalRenderPipeline_hpp
#define CPPMetalRenderPipeline_hpp

#include <CoreFoundation/CoreFoundation.h>
#include "CPPMetalImplementation.hpp"
#include "CPPMetalPixelFormat.hpp"
#include "CPPMetalConstants.hpp"
#include "CPPMetalDevice.hpp"
#include "CPPMetalVertexDescriptor.hpp"
#include "CPPMetalLibrary.hpp"
#include "CPPMetalTypes.hpp"


namespace MTL
{


typedef enum
{
    BlendFactorZero = 0,
    BlendFactorOne = 1,
    BlendFactorSourceColor = 2,
    BlendFactorOneMinusSourceColor = 3,
    BlendFactorSourceAlpha = 4,
    BlendFactorOneMinusSourceAlpha = 5,
    BlendFactorDestinationColor = 6,
    BlendFactorOneMinusDestinationColor = 7,
    BlendFactorDestinationAlpha = 8,
    BlendFactorOneMinusDestinationAlpha = 9,
    BlendFactorSourceAlphaSaturated = 10,
    BlendFactorBlendColor = 11,
    BlendFactorOneMinusBlendColor = 12,
    BlendFactorBlendAlpha = 13,
    BlendFactorOneMinusBlendAlpha = 14,
    BlendFactorSource1Color              API_AVAILABLE(macos(10.12), ios(10.11)) = 15,
    BlendFactorOneMinusSource1Color      API_AVAILABLE(macos(10.12), ios(10.11)) = 16,
    BlendFactorSource1Alpha              API_AVAILABLE(macos(10.12), ios(10.11)) = 17,
    BlendFactorOneMinusSource1Alpha      API_AVAILABLE(macos(10.12), ios(10.11)) = 18,
} BlendFactor;

typedef enum
{
    BlendOperationAdd = 0,
    BlendOperationSubtract = 1,
    BlendOperationReverseSubtract = 2,
    BlendOperationMin = 3,
    BlendOperationMax = 4,
} BlendOperation API_AVAILABLE(macos(10.11), ios(8.0));

class RenderPipelineColorAttachmentDescriptorArray;

struct RenderPipelineColorAttachmentDescriptor
{
public:

    RenderPipelineColorAttachmentDescriptor();

    RenderPipelineColorAttachmentDescriptor(const RenderPipelineColorAttachmentDescriptor & rhs);

    RenderPipelineColorAttachmentDescriptor & operator=(const RenderPipelineColorAttachmentDescriptor & rhs);

    CPP_METAL_VIRTUAL ~RenderPipelineColorAttachmentDescriptor();

    PixelFormat pixelFormat() const;
    void        pixelFormat(PixelFormat format);

    bool isBlendingEnabled() const;
    void blendingEnabled(bool enabled);

    BlendFactor sourceRGBBlendFactor() const;
    void        sourceRGBBlendFactor(BlendFactor factor);

    BlendFactor destinationRGBBlendFactor() const;
    void        destinationRGBBlendFactor(BlendFactor factor);

    BlendOperation rgbBlendOperation() const;
    void           rgbBlendOperation(BlendOperation operation);

    BlendFactor sourceAlphaBlendFactor() const;
    void        sourceAlphaBlendFactor(BlendFactor factor);

    BlendFactor destinationAlphaBlendFactor() const;
    void        destinationAlphaBlendFactor(BlendFactor factor);

    BlendOperation alphaBlendOperation() const;
    void           alphaBlendOperation(BlendOperation operation);

private:

    CPPMetalInternal::RenderPipelineColorAttachmentDescriptor m_objCObj;

    CPPMetalInternal::RenderPipelineColorAttachmentDescriptorArray m_objCContainer;

    UInteger m_arrayIndex;

    RenderPipelineColorAttachmentDescriptor(CPPMetalInternal::RenderPipelineColorAttachmentDescriptorArray objCContainer, UInteger arrayIndex);

    void reinitialize(CPPMetalInternal::RenderPipelineColorAttachmentDescriptorArray objCContainer);

    friend RenderPipelineColorAttachmentDescriptorArray;

public: // Public methods for CPPMetal internal implementation

    CPPMetalInternal::RenderPipelineColorAttachmentDescriptor objCObj() const;

};

class RenderPipelineColorAttachmentDescriptorArray
{
public:

    RenderPipelineColorAttachmentDescriptorArray() = delete;

    RenderPipelineColorAttachmentDescriptorArray(const RenderPipelineColorAttachmentDescriptorArray & rhs) = delete;

    RenderPipelineColorAttachmentDescriptorArray & operator=(const RenderPipelineColorAttachmentDescriptorArray & rhs) = delete;

    CPP_METAL_VIRTUAL ~RenderPipelineColorAttachmentDescriptorArray();

    RenderPipelineColorAttachmentDescriptor operator[](int i) const;

    RenderPipelineColorAttachmentDescriptor & operator[](int i);

private:

    CPPMetalInternal::RenderPipelineColorAttachmentDescriptorArray m_objCObj;

    RenderPipelineColorAttachmentDescriptor m_colorAttachments[MaxColorAttachments];

    explicit RenderPipelineColorAttachmentDescriptorArray(CPPMetalInternal::RenderPipelineColorAttachmentDescriptorArray objCObj);

    void reinitalize(CPPMetalInternal::RenderPipelineColorAttachmentDescriptorArray objCObj);

    friend RenderPipelineDescriptor; // To permit calling reinitialize in assignment operator

public: // Public methods for CPPMetal internal implementation

    CPPMetalInternal::RenderPipelineColorAttachmentDescriptorArray objCObj() const;

};

class Function;

struct RenderPipelineDescriptor
{
private:

    // CPPMetal Internal Implementation Note:
    // The class must declare the 'm_objCObj' member before the 'colorAttachments' member.
    // The constructor of the 'colorAttachments' member depend on the value of properties in
    // 'm_objCObj'.  C++ executes member constructors in order of declaration (not in order of the
    // initializer list in the contstructor).  So the class must declare 'm_objCObj' first for it to
    // initialize it first and allow 'colorAttachments' to depend on its properties.
    CPPMetalInternal::RenderPipelineDescriptor m_objCObj;

public:

    RenderPipelineDescriptor();

    RenderPipelineDescriptor(const RenderPipelineDescriptor & rhs);

    RenderPipelineDescriptor & operator=(const RenderPipelineDescriptor & rhs);

    CPP_METAL_VIRTUAL ~RenderPipelineDescriptor();

    bool operator==(const RenderPipelineDescriptor & rhs) const;

    const char* label() const;
    void        label(const CFStringRef string);
    void        label(const char* string);

    const Function *vertexFunction() const;
    void            vertexFunction(const Function * function);

    const Function *fragmentFunction() const;
    void            fragmentFunction(const Function * function);

    VertexDescriptor *vertexDescriptor();
    void              vertexDescriptor(VertexDescriptor * descriptor);

    UInteger sampleCount() const;
    void     sampleCount(UInteger count);

    PixelFormat depthAttachmentPixelFormat() const;
    void        depthAttachmentPixelFormat(PixelFormat format);

    PixelFormat stencilAttachmentPixelFormat() const;
    void        stencilAttachmentPixelFormat(PixelFormat format);

    RenderPipelineColorAttachmentDescriptorArray colorAttachments;

private:

    Function m_fragmentFunction;

    Function m_vertexFunction;

    VertexDescriptor m_vertexDescriptor;

public: // Public methods for CPPMetal internal implementation

    CPPMetalInternal::RenderPipelineDescriptor objCObj() const;

};

class Device;

class RenderPipelineState
{
public:

    RenderPipelineState();

    RenderPipelineState(const RenderPipelineState & rhs);

    RenderPipelineState(RenderPipelineState && rhs);

    RenderPipelineState & operator=(const RenderPipelineState & rhs);

    RenderPipelineState & operator=(RenderPipelineState && rhs);

    CPP_METAL_VIRTUAL ~RenderPipelineState();

    bool operator==(const RenderPipelineState & rhs) const;

    const char* label() const;

    Device device() const;

private:

    CPPMetalInternal::RenderPipelineState m_objCObj;

    Device *m_device;

public: // Public methods for CPPMetal internal implementation

    RenderPipelineState(CPPMetalInternal::RenderPipelineState objCObj, Device & device);

    CPPMetalInternal::RenderPipelineState objCObj() const;

};

//===========================================================================
#pragma mark - RenderPipelineColorAttachmentDescriptor inline implementations

inline CPPMetalInternal::RenderPipelineColorAttachmentDescriptor RenderPipelineColorAttachmentDescriptor::objCObj() const
{
    return m_objCObj;
}


//================================================================================
#pragma mark - RenderPipelineColorAttachmentDescriptorArray inline implementations

inline RenderPipelineColorAttachmentDescriptor RenderPipelineColorAttachmentDescriptorArray::operator[](int i) const
{
    return m_colorAttachments[i];
}

inline RenderPipelineColorAttachmentDescriptor & RenderPipelineColorAttachmentDescriptorArray::operator[](int i)
{
    return m_colorAttachments[i];
}

inline CPPMetalInternal::RenderPipelineColorAttachmentDescriptorArray RenderPipelineColorAttachmentDescriptorArray::objCObj() const
{
    return m_objCObj;
}


//============================================================
#pragma mark - RenderPipelineDescriptor inline implementations

inline void RenderPipelineDescriptor::label(const char* string)
{
    CPP_METAL_PROCESS_LABEL(string, label);
}

inline CPPMetalInternal::RenderPipelineDescriptor RenderPipelineDescriptor::objCObj() const
{
    return m_objCObj;
}


//=======================================================
#pragma mark - RenderPipelineState inline implementations

CPP_METAL_MOVE_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(RenderPipelineState);

CPP_METAL_OBJCOBJ_GETTER_IMPLEMENATATION(RenderPipelineState);


} // namespace MTL

#endif // CPPMetalRenderPipeline_hpp
