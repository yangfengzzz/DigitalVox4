//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cpp_mtl_RenderPipeline_hpp
#define cpp_mtl_RenderPipeline_hpp

#include <CoreFoundation/CoreFoundation.h>
#include "cpp_mtl_implementation.h"
#include "cpp_mtl_pixel_format.h"
#include "cpp_mtl_constants.h"
#include "cpp_mtl_device.h"
#include "cpp_mtl_vertex_descriptor.h"
#include "cpp_mtl_library.h"
#include "cpp_mtl_types.h"
#include "shader/shader_uniform.h"
#include "shader/shader_data_group.h"
#include <optional>

namespace MTL {
typedef enum {
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

typedef enum {
    ColorWriteMaskNone  = 0,
    ColorWriteMaskRed   = 0x1 << 3,
    ColorWriteMaskGreen = 0x1 << 2,
    ColorWriteMaskBlue  = 0x1 << 1,
    ColorWriteMaskAlpha = 0x1 << 0,
    ColorWriteMaskAll   = 0xf
} ColorWriteMask API_AVAILABLE(macos(10.11), ios(8.0));

typedef enum {
    BlendOperationAdd = 0,
    BlendOperationSubtract = 1,
    BlendOperationReverseSubtract = 2,
    BlendOperationMin = 3,
    BlendOperationMax = 4,
} BlendOperation API_AVAILABLE(macos(10.11), ios(8.0));

class RenderPipelineColorAttachmentDescriptorArray;

struct RenderPipelineColorAttachmentDescriptor {
public:
    
    RenderPipelineColorAttachmentDescriptor();
    
    RenderPipelineColorAttachmentDescriptor(const RenderPipelineColorAttachmentDescriptor &rhs);
    
    RenderPipelineColorAttachmentDescriptor &operator=(const RenderPipelineColorAttachmentDescriptor &rhs);
    
    CPP_METAL_VIRTUAL ~RenderPipelineColorAttachmentDescriptor();
    
    PixelFormat pixelFormat() const;
    
    void pixelFormat(PixelFormat format);
    
    bool isBlendingEnabled() const;
    
    void blendingEnabled(bool enabled);
    
    BlendFactor sourceRGBBlendFactor() const;
    
    void sourceRGBBlendFactor(BlendFactor factor);
    
    BlendFactor destinationRGBBlendFactor() const;
    
    void destinationRGBBlendFactor(BlendFactor factor);
    
    BlendOperation rgbBlendOperation() const;
    
    void rgbBlendOperation(BlendOperation operation);
    
    BlendFactor sourceAlphaBlendFactor() const;
    
    void sourceAlphaBlendFactor(BlendFactor factor);
    
    BlendFactor destinationAlphaBlendFactor() const;
    
    void destinationAlphaBlendFactor(BlendFactor factor);
    
    BlendOperation alphaBlendOperation() const;
    
    void alphaBlendOperation(BlendOperation operation);
    
    ColorWriteMask writeMask() const;
    
    void writeMask(ColorWriteMask mask);
    
private:
    
    cpp_mtl_internal::RenderPipelineColorAttachmentDescriptor m_objCObj;
    
    cpp_mtl_internal::RenderPipelineColorAttachmentDescriptorArray m_objCContainer;
    
    UInteger m_arrayIndex;
    
    RenderPipelineColorAttachmentDescriptor(cpp_mtl_internal::RenderPipelineColorAttachmentDescriptorArray objCContainer, UInteger arrayIndex);
    
    void reinitialize(cpp_mtl_internal::RenderPipelineColorAttachmentDescriptorArray objCContainer);
    
    friend RenderPipelineColorAttachmentDescriptorArray;
    
public: // Public methods for cpp_mtl_ internal implementation
    
    cpp_mtl_internal::RenderPipelineColorAttachmentDescriptor objCObj() const;
    
};

class RenderPipelineColorAttachmentDescriptorArray {
public:
    
    RenderPipelineColorAttachmentDescriptorArray() = delete;
    
    RenderPipelineColorAttachmentDescriptorArray(const RenderPipelineColorAttachmentDescriptorArray &rhs) = delete;
    
    RenderPipelineColorAttachmentDescriptorArray &operator=(const RenderPipelineColorAttachmentDescriptorArray &rhs) = delete;
    
    CPP_METAL_VIRTUAL ~RenderPipelineColorAttachmentDescriptorArray();
    
    RenderPipelineColorAttachmentDescriptor operator[](int i) const;
    
    RenderPipelineColorAttachmentDescriptor &operator[](int i);
    
private:
    
    cpp_mtl_internal::RenderPipelineColorAttachmentDescriptorArray m_objCObj;
    
    RenderPipelineColorAttachmentDescriptor m_colorAttachments[MaxColorAttachments];
    
    explicit RenderPipelineColorAttachmentDescriptorArray(cpp_mtl_internal::RenderPipelineColorAttachmentDescriptorArray objCObj);
    
    void reinitalize(cpp_mtl_internal::RenderPipelineColorAttachmentDescriptorArray objCObj);
    
    friend RenderPipelineDescriptor; // To permit calling reinitialize in assignment operator
    
public: // Public methods for cpp_mtl_ internal implementation
    
    cpp_mtl_internal::RenderPipelineColorAttachmentDescriptorArray objCObj() const;
    
};

class Function;

struct RenderPipelineDescriptor {
private:
    
    // cpp_mtl_ Internal Implementation Note:
    // The class must declare the 'm_objCObj' member before the 'colorAttachments' member.
    // The constructor of the 'colorAttachments' member depend on the value of properties in
    // 'm_objCObj'.  C++ executes member constructors in order of declaration (not in order of the
    // initializer list in the contstructor).  So the class must declare 'm_objCObj' first for it to
    // initialize it first and allow 'colorAttachments' to depend on its properties.
    cpp_mtl_internal::RenderPipelineDescriptor m_objCObj;
    
public:
    
    RenderPipelineDescriptor();
    
    RenderPipelineDescriptor(const RenderPipelineDescriptor &rhs);
    
    RenderPipelineDescriptor &operator=(const RenderPipelineDescriptor &rhs);
    
    CPP_METAL_VIRTUAL ~RenderPipelineDescriptor();
    
    bool operator==(const RenderPipelineDescriptor &rhs) const;
    
    const char *label() const;
    
    void label(const CFStringRef string);
    
    void label(const char *string);
    
    const Function *vertexFunction() const;
    
    void vertexFunction(const Function *function);
    
    const Function *fragmentFunction() const;
    
    void fragmentFunction(const Function *function);
    
    const VertexDescriptor *vertexDescriptor() const;
    
    void vertexDescriptor(const VertexDescriptor *descriptor);
    
    UInteger sampleCount() const;
    
    void sampleCount(UInteger count);
    
    PixelFormat depthAttachmentPixelFormat() const;
    
    void depthAttachmentPixelFormat(PixelFormat format);
    
    PixelFormat stencilAttachmentPixelFormat() const;
    
    void stencilAttachmentPixelFormat(PixelFormat format);
    
    RenderPipelineColorAttachmentDescriptorArray colorAttachments;
    
    bool isAlphaToCoverageEnabled() const;
    
    void alphaToCoverageEnabled(bool enabled);
    
private:
    
    Function m_fragmentFunction;
    
    Function m_vertexFunction;
    
    VertexDescriptor m_vertexDescriptor;
    
public: // Public methods for cpp_mtl_ internal implementation
    
    cpp_mtl_internal::RenderPipelineDescriptor objCObj() const;
    
};

class Device;

class RenderPipelineState {
public:
    std::vector<vox::ShaderUniform> sceneUniformBlock{};
    std::vector<vox::ShaderUniform> cameraUniformBlock{};
    std::vector<vox::ShaderUniform> rendererUniformBlock{};
    std::vector<vox::ShaderUniform> materialUniformBlock{};
    
    RenderPipelineState();
    
    RenderPipelineState(const RenderPipelineState &rhs);
    
    RenderPipelineState(RenderPipelineState &&rhs);
    
    RenderPipelineState &operator=(const RenderPipelineState &rhs);
    
    RenderPipelineState &operator=(RenderPipelineState &&rhs);
    
    CPP_METAL_VIRTUAL ~RenderPipelineState();
    
    bool operator==(const RenderPipelineState &rhs) const;
    
    const char *label() const;
    
    const Device* device() const;
    
private:
    /**
     * record the location of uniform/attribute.
     */
    void _recordVertexLocation(cpp_mtl_internal::RenderPipelineReflection reflection);
    
    void _groupingUniform(const vox::ShaderUniform &uniform,
                          const std::optional<vox::ShaderDataGroup> &group);
    
    cpp_mtl_internal::RenderPipelineState m_objCObj;
    
    Device *m_device;
    
public: // Public methods for cpp_mtl_ internal implementation
    RenderPipelineState(cpp_mtl_internal::RenderPipelineState objCObj,
                        cpp_mtl_internal::RenderPipelineReflection reflection,
                        Device &device);
    
    cpp_mtl_internal::RenderPipelineState objCObj() const;
};

#pragma mark - RenderPipelineColorAttachmentDescriptor inline implementations
inline cpp_mtl_internal::RenderPipelineColorAttachmentDescriptor RenderPipelineColorAttachmentDescriptor::objCObj() const {
    return m_objCObj;
}


#pragma mark - RenderPipelineColorAttachmentDescriptorArray inline implementations
inline RenderPipelineColorAttachmentDescriptor RenderPipelineColorAttachmentDescriptorArray::operator[](int i) const {
    return m_colorAttachments[i];
}

inline RenderPipelineColorAttachmentDescriptor &RenderPipelineColorAttachmentDescriptorArray::operator[](int i) {
    return m_colorAttachments[i];
}

inline cpp_mtl_internal::RenderPipelineColorAttachmentDescriptorArray RenderPipelineColorAttachmentDescriptorArray::objCObj() const {
    return m_objCObj;
}


#pragma mark - RenderPipelineDescriptor inline implementations
inline void RenderPipelineDescriptor::label(const char *string) {
    CPP_METAL_PROCESS_LABEL(string, label);
}

inline cpp_mtl_internal::RenderPipelineDescriptor RenderPipelineDescriptor::objCObj() const {
    return m_objCObj;
}


#pragma mark - RenderPipelineState inline implementations
inline RenderPipelineState::RenderPipelineState(RenderPipelineState && rhs)
: m_objCObj(rhs.m_objCObj)
, m_device(rhs.m_device),
sceneUniformBlock(rhs.sceneUniformBlock),
cameraUniformBlock(rhs.cameraUniformBlock),
rendererUniformBlock(rhs.rendererUniformBlock),
materialUniformBlock(rhs.materialUniformBlock) {
    rhs.m_objCObj = nullptr;
    rhs.m_device = nullptr;
}

inline RenderPipelineState & RenderPipelineState::operator=(RenderPipelineState && rhs) {
    m_objCObj = rhs.m_objCObj;
    m_device = rhs.m_device;
    sceneUniformBlock = rhs.sceneUniformBlock;
    cameraUniformBlock = rhs.cameraUniformBlock;
    rendererUniformBlock = rhs.rendererUniformBlock;
    materialUniformBlock = rhs.materialUniformBlock;
    
    rhs.m_objCObj = nullptr;
    rhs.m_device = nullptr;
    
    return *this;
}

CPP_METAL_OBJCOBJ_GETTER_IMPLEMENATATION(RenderPipelineState);


} // namespace MTL

#endif // cpp_mtl_RenderPipeline_hpp
