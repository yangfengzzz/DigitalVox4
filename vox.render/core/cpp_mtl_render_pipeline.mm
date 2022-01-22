//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.


#include "cpp_mtl_render_pipeline.h"
#include "cpp_mtl_library.h"
#include "cpp_mtl_device.h"
#include "cpp_mtl_internal_macros.h"
#include "shader/shader.h"

using namespace MTL;

#pragma mark - Enum Validation

CPP_METAL_VALIDATE_ENUM_ALIAS(BlendFactorZero);
CPP_METAL_VALIDATE_ENUM_ALIAS(BlendFactorOne);
CPP_METAL_VALIDATE_ENUM_ALIAS(BlendFactorSourceColor);
CPP_METAL_VALIDATE_ENUM_ALIAS(BlendFactorOneMinusSourceColor);
CPP_METAL_VALIDATE_ENUM_ALIAS(BlendFactorSourceAlpha);
CPP_METAL_VALIDATE_ENUM_ALIAS(BlendFactorOneMinusSourceAlpha);
CPP_METAL_VALIDATE_ENUM_ALIAS(BlendFactorDestinationColor);
CPP_METAL_VALIDATE_ENUM_ALIAS(BlendFactorOneMinusDestinationColor);
CPP_METAL_VALIDATE_ENUM_ALIAS(BlendFactorDestinationAlpha);
CPP_METAL_VALIDATE_ENUM_ALIAS(BlendFactorOneMinusDestinationAlpha);
CPP_METAL_VALIDATE_ENUM_ALIAS(BlendFactorSourceAlphaSaturated);
CPP_METAL_VALIDATE_ENUM_ALIAS(BlendFactorBlendColor);
CPP_METAL_VALIDATE_ENUM_ALIAS(BlendFactorOneMinusBlendColor);
CPP_METAL_VALIDATE_ENUM_ALIAS(BlendFactorBlendAlpha);
CPP_METAL_VALIDATE_ENUM_ALIAS(BlendFactorOneMinusBlendAlpha);
CPP_METAL_VALIDATE_ENUM_ALIAS(BlendFactorSource1Color);
CPP_METAL_VALIDATE_ENUM_ALIAS(BlendFactorOneMinusSource1Color);
CPP_METAL_VALIDATE_ENUM_ALIAS(BlendFactorSource1Alpha);
CPP_METAL_VALIDATE_ENUM_ALIAS(BlendFactorOneMinusSource1Alpha);


CPP_METAL_VALIDATE_ENUM_ALIAS(BlendOperationAdd);
CPP_METAL_VALIDATE_ENUM_ALIAS(BlendOperationSubtract);
CPP_METAL_VALIDATE_ENUM_ALIAS(BlendOperationReverseSubtract);
CPP_METAL_VALIDATE_ENUM_ALIAS(BlendOperationMin);
CPP_METAL_VALIDATE_ENUM_ALIAS(BlendOperationMax);

#pragma mark - RenderPipelineColorAttachmentDescriptor

RenderPipelineColorAttachmentDescriptor::RenderPipelineColorAttachmentDescriptor() :
m_objCObj([MTLRenderPipelineColorAttachmentDescriptor new]),
m_objCContainer(nullptr),
m_arrayIndex(MaxColorAttachments) {
    // Member initialization only
}


RenderPipelineColorAttachmentDescriptor::RenderPipelineColorAttachmentDescriptor(cpp_mtl_internal::RenderPipelineColorAttachmentDescriptorArray objCContainer, UInteger arrayIndex)
:
m_objCObj(objCContainer[arrayIndex]),
m_objCContainer(objCContainer),
m_arrayIndex(arrayIndex) {
    // Member initialization only
}


RenderPipelineColorAttachmentDescriptor::RenderPipelineColorAttachmentDescriptor(const RenderPipelineColorAttachmentDescriptor &rhs)
:
m_objCObj([rhs.m_objCObj copyWithZone:nil]),
m_objCContainer(nullptr),
m_arrayIndex(MaxColorAttachments) {
    // Member initialization only
}


RenderPipelineColorAttachmentDescriptor &RenderPipelineColorAttachmentDescriptor::operator=(const RenderPipelineColorAttachmentDescriptor &rhs) {
    if (m_objCContainer) {
        m_objCContainer[m_arrayIndex] = rhs.m_objCObj;
        m_objCObj = m_objCContainer[m_arrayIndex];
    } else {
        m_objCObj = [rhs.m_objCObj copyWithZone:nil];
    }
    
    return *this;
}


void RenderPipelineColorAttachmentDescriptor::reinitialize(cpp_mtl_internal::RenderPipelineColorAttachmentDescriptorArray objCContainer) {
    m_objCObj = objCContainer[m_arrayIndex];
    m_objCContainer = objCContainer;
}

RenderPipelineColorAttachmentDescriptor::~RenderPipelineColorAttachmentDescriptor() {
    m_objCObj = nil;
    m_objCContainer = nil;
}

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(RenderPipelineColorAttachmentDescriptor, PixelFormat, pixelFormat);

CPP_METAL_READWRITE_BOOL_PROPERTY_IMPLEMENTATION(RenderPipelineColorAttachmentDescriptor, blendingEnabled, isBlendingEnabled);

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(RenderPipelineColorAttachmentDescriptor, BlendFactor, sourceRGBBlendFactor);

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(RenderPipelineColorAttachmentDescriptor, BlendFactor, destinationRGBBlendFactor);

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(RenderPipelineColorAttachmentDescriptor, BlendOperation, rgbBlendOperation);

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(RenderPipelineColorAttachmentDescriptor, BlendFactor, sourceAlphaBlendFactor);

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(RenderPipelineColorAttachmentDescriptor, BlendFactor, destinationAlphaBlendFactor);

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(RenderPipelineColorAttachmentDescriptor, BlendOperation, alphaBlendOperation);

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(RenderPipelineColorAttachmentDescriptor, ColorWriteMask, writeMask);

//========================================================================
#pragma mark - RenderPipelineColorAttachmentDescriptorArray Implemeentation

RenderPipelineColorAttachmentDescriptorArray::RenderPipelineColorAttachmentDescriptorArray(cpp_mtl_internal::RenderPipelineColorAttachmentDescriptorArray objCObj)
:
m_objCObj(objCObj),
m_colorAttachments{
    {objCObj, 0},
    {objCObj, 1},
    {objCObj, 2},
    {objCObj, 3},
    {objCObj, 4},
    {objCObj, 5},
    {objCObj, 6},
    {objCObj, 7}
} {
    // Validate that all attachments have been set in the correct index
    for (int i = 0; i < MaxColorAttachments; i++) {
        assert(m_colorAttachments[i].objCObj() == objCObj[i]);
    }
}

RenderPipelineColorAttachmentDescriptorArray::~RenderPipelineColorAttachmentDescriptorArray() {
    m_objCObj = nil;
}

void RenderPipelineColorAttachmentDescriptorArray::reinitalize(cpp_mtl_internal::RenderPipelineColorAttachmentDescriptorArray objCObj) {
    m_objCObj = objCObj;
    
    for (int i = 0; i < MaxColorAttachments; i++) {
        m_colorAttachments[i].reinitialize(objCObj);
    }
}

//====================================================
#pragma mark - RenderPipelineDescriptor Implementation

_Static_assert(8 == MaxColorAttachments, "Initializing only 8 attachments in MTL::RenderPipelineDescritptor");

RenderPipelineDescriptor::RenderPipelineDescriptor()
: m_objCObj([MTLRenderPipelineDescriptor new]), colorAttachments(m_objCObj.colorAttachments) {
    // Member initialization only
}

RenderPipelineDescriptor::RenderPipelineDescriptor(const RenderPipelineDescriptor &rhs)
: m_objCObj([rhs.m_objCObj copyWithZone:nil]),
m_vertexFunction(rhs.m_vertexFunction),
m_fragmentFunction(rhs.m_fragmentFunction),
m_vertexDescriptor(rhs.m_vertexDescriptor),
colorAttachments(m_objCObj.colorAttachments) {
    
    // Member initialization only
}


RenderPipelineDescriptor &RenderPipelineDescriptor::operator=(const RenderPipelineDescriptor &rhs) {
    m_objCObj = [rhs.m_objCObj copyWithZone:nil];
    m_vertexFunction = rhs.m_vertexFunction;
    m_fragmentFunction = rhs.m_fragmentFunction;
    m_vertexDescriptor = rhs.m_vertexDescriptor;
    
    colorAttachments.reinitalize(m_objCObj.colorAttachments);
    
    return *this;
}

RenderPipelineDescriptor::~RenderPipelineDescriptor() {
    m_objCObj = nil;
}

CPP_METAL_READWRITE_LABEL_PROPERTY_IMPLEMENTATION(RenderPipelineDescriptor)


CPP_METAL_READWRITE_PROPERTY_IMPLEMENTATION(RenderPipelineDescriptor, UInteger, sampleCount);


void RenderPipelineDescriptor::vertexFunction(const Function *function) {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    if (function) {
        m_objCObj.vertexFunction = function->objCObj();
        m_vertexFunction = *function;
    } else {
        m_objCObj.vertexFunction = nil;
        m_vertexFunction = Function();
    }
}

const Function *RenderPipelineDescriptor::vertexFunction() const {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    if (m_objCObj.vertexFunction == nil) {
        assert(m_vertexFunction.objCObj() == nil);
        
        return nullptr;
    }
    return &m_vertexFunction;
}


// fragmentFunction
void RenderPipelineDescriptor::fragmentFunction(const Function *function) {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    if (function) {
        m_objCObj.fragmentFunction = function->objCObj();
        m_fragmentFunction = *function;
    } else {
        m_objCObj.fragmentFunction = nil;
        m_fragmentFunction = Function();
    }
}

const Function *RenderPipelineDescriptor::fragmentFunction() const {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    if (m_objCObj.fragmentFunction == nil) {
        assert(m_fragmentFunction.objCObj() == nil);
        
        return nullptr;
    }
    return &m_fragmentFunction;
}

void RenderPipelineDescriptor::vertexDescriptor(const VertexDescriptor *descriptor) {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    if (descriptor) {
        m_objCObj.vertexDescriptor = descriptor->objCObj();
        m_vertexDescriptor = *descriptor;
    } else {
        m_objCObj.vertexDescriptor = nil;
        m_vertexDescriptor = VertexDescriptor();
    }
}

const VertexDescriptor *RenderPipelineDescriptor::vertexDescriptor() const {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    if (m_objCObj.vertexDescriptor == nil) {
        return nullptr;
    }
    return &m_vertexDescriptor;
}

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(RenderPipelineDescriptor, PixelFormat, depthAttachmentPixelFormat);

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(RenderPipelineDescriptor, PixelFormat, stencilAttachmentPixelFormat)

CPP_METAL_READWRITE_BOOL_PROPERTY_IMPLEMENTATION(RenderPipelineDescriptor, alphaToCoverageEnabled, isAlphaToCoverageEnabled);

bool RenderPipelineDescriptor::operator==(const RenderPipelineDescriptor &rhs) const {
    return [m_objCObj isEqual:rhs.objCObj()];
}


#pragma mark - RenderPipelineState
RenderPipelineState::RenderPipelineState(cpp_mtl_internal::RenderPipelineState objCObj,
                                         cpp_mtl_internal::RenderPipelineReflection reflection,
                                         Device & device)
: m_objCObj(objCObj)
, m_device(&device)
{
    assert([m_device->objCObj() isEqual:m_objCObj.device]);
    _recordVertexLocation(reflection);
}

void RenderPipelineState::_recordVertexLocation(cpp_mtl_internal::RenderPipelineReflection reflection) {
    auto count = [[reflection vertexArguments] count];
    if (count != 0) {
        for (size_t i = 0; i < count; i++) {
            const auto &aug = [reflection vertexArguments][i];
            const auto name = [aug.name cStringUsingEncoding:NSUTF8StringEncoding];
            const auto location = aug.index;
            const auto group = vox::Shader::getShaderPropertyGroup(name);
            
            vox::ShaderUniform shaderUniform;
            shaderUniform.name = name;
            shaderUniform.propertyId = vox::Shader::getPropertyByName(name)->uniqueId;
            shaderUniform.location = location;
            shaderUniform.type = MTL::FunctionTypeVertex;
            _groupingUniform(shaderUniform, group);
        }
    }
    
    count = [[reflection fragmentArguments] count];
    if (count != 0) {
        for (size_t i = 0; i < count; i++) {
            const auto &aug = [reflection fragmentArguments][i];
            const auto name = [aug.name cStringUsingEncoding:NSUTF8StringEncoding];
            const auto location = aug.index;
            const auto group = vox::Shader::getShaderPropertyGroup(name);
            
            vox::ShaderUniform shaderUniform;
            shaderUniform.name = name;
            shaderUniform.propertyId = vox::Shader::getPropertyByName(name)->uniqueId;
            shaderUniform.location = location;
            shaderUniform.type = MTL::FunctionTypeFragment;
            _groupingUniform(shaderUniform, group);
        }
    }
}

void RenderPipelineState::_groupingUniform(const vox::ShaderUniform &uniform,
                                           const std::optional<vox::ShaderDataGroup> &group) {
    if (group != std::nullopt) {
        switch (group.value()) {
            case vox::ShaderDataGroup::Scene:
                sceneUniformBlock.push_back(uniform);
                break;
            case vox::ShaderDataGroup::Camera:
                cameraUniformBlock.push_back(uniform);
                break;
            case vox::ShaderDataGroup::Renderer:
                rendererUniformBlock.push_back(uniform);
                break;
            case vox::ShaderDataGroup::Material:
                materialUniformBlock.push_back(uniform);
                break;
        }
    } else {
        // std::cerr << "Unknown uniform group" << std::endl;
    }
}

CPP_METAL_NULL_REFERENCE_CONSTRUCTOR_IMPLEMENATATION(RenderPipelineState);

RenderPipelineState::RenderPipelineState(const RenderPipelineState & rhs)
: m_objCObj(rhs.m_objCObj),
m_device(rhs.m_device),
sceneUniformBlock(rhs.sceneUniformBlock),
cameraUniformBlock(rhs.cameraUniformBlock),
rendererUniformBlock(rhs.rendererUniformBlock),
materialUniformBlock(rhs.materialUniformBlock) {
    
}

RenderPipelineState & RenderPipelineState::operator=(const RenderPipelineState & rhs) {
    m_objCObj = rhs.m_objCObj;
    m_device = rhs.m_device;
    sceneUniformBlock = rhs.sceneUniformBlock;
    cameraUniformBlock = rhs.cameraUniformBlock;
    rendererUniformBlock = rhs.rendererUniformBlock;
    materialUniformBlock = rhs.materialUniformBlock;
    
    return *this;
}

RenderPipelineState::~RenderPipelineState() {
    m_objCObj = nil;
}

const char *RenderPipelineState::label() const {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    return m_objCObj.label.UTF8String;
}

bool RenderPipelineState::operator==(const RenderPipelineState &rhs) const {
    return [m_objCObj isEqual:rhs.objCObj()];
}

CPP_METAL_DEVICE_GETTER_IMPLEMENTATION(RenderPipelineState);
