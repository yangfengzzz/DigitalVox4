/*
 See LICENSE folder for this sample’s licensing information.
 
 Abstract:
 Implementation of C++ Metal vertex descriptor wrapper
 */


#include "CPPMetalVertexDescriptor.hpp"

#include "CPPMetalInternalMacros.h"

using namespace MTL;

//========================================================
#pragma mark - VertexBufferLayoutDescriptor Implementation

VertexBufferLayoutDescriptor::VertexBufferLayoutDescriptor() :
m_objCObj([MTLVertexBufferLayoutDescriptor new]),
m_objCContainer(nil),
m_arrayIndex(MaxVertexBufferLayouts) {
    // Member initialization only
}

VertexBufferLayoutDescriptor::VertexBufferLayoutDescriptor(CPPMetalInternal::VertexBufferLayoutDescriptorArray objCContainer, UInteger arrayIndex)
:
m_objCObj(objCContainer[arrayIndex]),
m_objCContainer(objCContainer),
m_arrayIndex(arrayIndex) {
    // Member initialization only
}


VertexBufferLayoutDescriptor::VertexBufferLayoutDescriptor(const VertexBufferLayoutDescriptor &rhs) :
m_objCObj([rhs.m_objCObj copyWithZone:nil]),
m_objCContainer(nullptr),
m_arrayIndex(MaxVertexBufferLayouts) {
    // Member initialization only
}


VertexBufferLayoutDescriptor::~VertexBufferLayoutDescriptor() {
    m_objCObj = nil;
}

VertexBufferLayoutDescriptor &VertexBufferLayoutDescriptor::operator=(const VertexBufferLayoutDescriptor &rhs) {
    if (m_objCContainer) {
        m_objCContainer[m_arrayIndex] = rhs.m_objCObj;
        m_objCObj = m_objCContainer[m_arrayIndex];
    } else {
        m_objCObj = [rhs.m_objCObj copyWithZone:nil];
    }
    
    return *this;
}

void VertexBufferLayoutDescriptor::reinitialize(CPPMetalInternal::VertexBufferLayoutDescriptorArray objCContainer) {
    m_objCObj = objCContainer[m_arrayIndex];
    m_objCContainer = objCContainer;
}

CPP_METAL_READWRITE_PROPERTY_IMPLEMENTATION(VertexBufferLayoutDescriptor, UInteger, stride);

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(VertexBufferLayoutDescriptor, VertexStepFunction, stepFunction);

CPP_METAL_READWRITE_PROPERTY_IMPLEMENTATION(VertexBufferLayoutDescriptor, UInteger, stepRate);


//=====================================================
#pragma mark - VertexAttributeDescriptor Implementation

VertexAttributeDescriptor::VertexAttributeDescriptor() :
m_objCObj([MTLVertexAttributeDescriptor new]),
m_objCContainer(nil),
m_arrayIndex(MaxVertexAttributes) {
    // Member initialization only
}

VertexAttributeDescriptor::VertexAttributeDescriptor(const VertexAttributeDescriptor &rhs) :
m_objCObj([rhs.m_objCObj copyWithZone:nil]),
m_objCContainer(nullptr),
m_arrayIndex(MaxVertexAttributes) {
    // Member initialization only
}

VertexAttributeDescriptor::VertexAttributeDescriptor(CPPMetalInternal::VertexAttributeDescriptorArray objCContainer, UInteger arrayIndex)
:
m_objCObj(objCContainer[arrayIndex]),
m_objCContainer(objCContainer),
m_arrayIndex(arrayIndex) {
    // Member initialization only
}


VertexAttributeDescriptor::~VertexAttributeDescriptor() {
    m_objCObj = nil;
}


VertexAttributeDescriptor &VertexAttributeDescriptor::operator=(const VertexAttributeDescriptor &rhs) {
    if (m_objCContainer) {
        m_objCContainer[m_arrayIndex] = rhs.m_objCObj;
        m_objCObj = m_objCContainer[m_arrayIndex];
    } else {
        m_objCObj = [rhs.m_objCObj copyWithZone:nil];
    }
    
    return *this;
}

void VertexAttributeDescriptor::reinitialize(CPPMetalInternal::VertexAttributeDescriptorArray objCContainer) {
    m_objCObj = objCContainer[m_arrayIndex];
    m_objCContainer = objCContainer;
}

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(VertexAttributeDescriptor, VertexFormat, format);

CPP_METAL_READWRITE_PROPERTY_IMPLEMENTATION(VertexAttributeDescriptor, UInteger, offset);

CPP_METAL_READWRITE_PROPERTY_IMPLEMENTATION(VertexAttributeDescriptor, UInteger, bufferIndex);


//=============================================================
#pragma mark - VertexBufferLayoutDescriptorArray Implementation

VertexBufferLayoutDescriptorArray::VertexBufferLayoutDescriptorArray(CPPMetalInternal::VertexBufferLayoutDescriptorArray objCObj)
:
m_objCObj(objCObj),
m_layouts
{
    {objCObj, 0},
    {objCObj, 1},
    {objCObj, 2},
    {objCObj, 3},
    {objCObj, 4},
    {objCObj, 5},
    {objCObj, 6},
    {objCObj, 7},
    {objCObj, 8},
    {objCObj, 9},
    {objCObj, 10},
    {objCObj, 11},
    {objCObj, 12},
    {objCObj, 13},
    {objCObj, 14},
    {objCObj, 15},
    {objCObj, 16},
    {objCObj, 17},
    {objCObj, 18},
    {objCObj, 19},
    {objCObj, 20},
    {objCObj, 21},
    {objCObj, 22},
    {objCObj, 23},
    {objCObj, 24},
    {objCObj, 25},
    {objCObj, 26},
    {objCObj, 27},
    {objCObj, 28},
    {objCObj, 29},
    {objCObj, 30},
} {
    for (int i = 0; i < MaxVertexBufferLayouts; i++) {
        assert(m_objCObj[i] == m_layouts[i].objCObj());
    }
}


VertexBufferLayoutDescriptorArray::~VertexBufferLayoutDescriptorArray() {
    m_objCObj = nil;
}

void VertexBufferLayoutDescriptorArray::reinitialize(CPPMetalInternal::VertexBufferLayoutDescriptorArray objCObj) {
    for (int i = 0; i < MaxVertexBufferLayouts; i++) {
        m_layouts[i].reinitialize(m_objCObj);
    }
}


//==========================================================
#pragma mark - VertexAttributeDescriptorArray Implementation

VertexAttributeDescriptorArray::VertexAttributeDescriptorArray(CPPMetalInternal::VertexAttributeDescriptorArray objCObj)
:
m_objCObj(objCObj),
m_attributes
{
    {objCObj, 0},
    {objCObj, 1},
    {objCObj, 2},
    {objCObj, 3},
    {objCObj, 4},
    {objCObj, 5},
    {objCObj, 6},
    {objCObj, 7},
    {objCObj, 8},
    {objCObj, 9},
    {objCObj, 10},
    {objCObj, 11},
    {objCObj, 12},
    {objCObj, 13},
    {objCObj, 14},
    {objCObj, 15},
    {objCObj, 16},
    {objCObj, 17},
    {objCObj, 18},
    {objCObj, 19},
    {objCObj, 20},
    {objCObj, 21},
    {objCObj, 22},
    {objCObj, 23},
    {objCObj, 24},
    {objCObj, 25},
    {objCObj, 26},
    {objCObj, 27},
    {objCObj, 28},
    {objCObj, 29},
    {objCObj, 30},
} {
    for (int i = 0; i < MaxVertexAttributes; i++) {
        assert(m_objCObj[i] == m_attributes[i].objCObj());
    }
}

VertexAttributeDescriptorArray::~VertexAttributeDescriptorArray() {
    m_objCObj = nil;
}

void VertexAttributeDescriptorArray::reinitialize(CPPMetalInternal::VertexAttributeDescriptorArray objCObj) {
    m_objCObj = objCObj;
    
    for (int i = 0; i < MaxVertexAttributes; i++) {
        m_attributes[i].reinitialize(m_objCObj);
    }
}


//============================================
#pragma mark - VertexDescriptor Implementation

VertexDescriptor::VertexDescriptor() :
m_objCObj([MTLVertexDescriptor new]),
layouts(m_objCObj.layouts),
attributes(m_objCObj.attributes) {
    // Member initialization only
}

VertexDescriptor::VertexDescriptor(const VertexDescriptor &rhs) :
m_objCObj(rhs.objCObj()),
layouts(m_objCObj.layouts),
attributes(m_objCObj.attributes) {
    // Member initialization only
}

VertexDescriptor &VertexDescriptor::operator=(const VertexDescriptor &rhs) {
    m_objCObj = rhs.objCObj();
    
    layouts.reinitialize(m_objCObj.layouts);
    
    attributes.reinitialize(m_objCObj.attributes);
    
    return *this;
}

VertexDescriptor::~VertexDescriptor() {
    m_objCObj = nil;
}

void VertexDescriptor::reset() {
    [m_objCObj reset];
}


//==================================
#pragma mark - Enum value validation

CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatInvalid);

CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatUChar2);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatUChar3);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatUChar4);

CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatChar2);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatChar3);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatChar4);

CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatUChar2Normalized);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatUChar3Normalized);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatUChar4Normalized);

CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatChar2Normalized);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatChar3Normalized);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatChar4Normalized);

CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatUShort2);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatUShort3);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatUShort4);

CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatShort2);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatShort3);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatShort4);

CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatUShort2Normalized);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatUShort3Normalized);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatUShort4Normalized);

CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatShort2Normalized);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatShort3Normalized);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatShort4Normalized);

CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatHalf2);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatHalf3);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatHalf4);

CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatFloat);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatFloat2);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatFloat3);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatFloat4);

CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatInt);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatInt2);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatInt3);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatInt4);

CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatUInt);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatUInt2);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatUInt3);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatUInt4);

CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatInt1010102Normalized);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatUInt1010102Normalized);

CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatUChar4Normalized_BGRA);

CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatUChar);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatChar);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatUCharNormalized);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatCharNormalized);

CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatUShort);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatShort);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatUShortNormalized);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatShortNormalized);

CPP_METAL_VALIDATE_ENUM_ALIAS(VertexFormatHalf);

CPP_METAL_VALIDATE_ENUM_ALIAS(VertexStepFunctionConstant);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexStepFunctionPerVertex);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexStepFunctionPerInstance);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexStepFunctionPerPatch);
CPP_METAL_VALIDATE_ENUM_ALIAS(VertexStepFunctionPerPatchControlPoint);
