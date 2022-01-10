/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Implementation of C++ Metal depth stencil state class wrappers
*/

#include <CoreFoundation/CoreFoundation.h>
#include "CPPMetalDepthStencil.hpp"
#include "CPPMetalInternalMacros.h"
#include "CPPMetalDevice.hpp"

using namespace MTL;

CPP_METAL_VALIDATE_ENUM_ALIAS( CompareFunctionNever );
CPP_METAL_VALIDATE_ENUM_ALIAS( CompareFunctionLess );
CPP_METAL_VALIDATE_ENUM_ALIAS( CompareFunctionEqual );
CPP_METAL_VALIDATE_ENUM_ALIAS( CompareFunctionLessEqual );
CPP_METAL_VALIDATE_ENUM_ALIAS( CompareFunctionGreater );
CPP_METAL_VALIDATE_ENUM_ALIAS( CompareFunctionNotEqual );
CPP_METAL_VALIDATE_ENUM_ALIAS( CompareFunctionGreaterEqual );
CPP_METAL_VALIDATE_ENUM_ALIAS( CompareFunctionAlways );

CPP_METAL_VALIDATE_ENUM_ALIAS( StencilOperationKeep );
CPP_METAL_VALIDATE_ENUM_ALIAS( StencilOperationZero );
CPP_METAL_VALIDATE_ENUM_ALIAS( StencilOperationReplace );
CPP_METAL_VALIDATE_ENUM_ALIAS( StencilOperationIncrementClamp );
CPP_METAL_VALIDATE_ENUM_ALIAS( StencilOperationDecrementClamp );
CPP_METAL_VALIDATE_ENUM_ALIAS( StencilOperationInvert );
CPP_METAL_VALIDATE_ENUM_ALIAS( StencilOperationIncrementWrap );
CPP_METAL_VALIDATE_ENUM_ALIAS( StencilOperationDecrementWrap );

#pragma mark - StencilDescriptor

StencilDescriptor::StencilDescriptor() :
m_objCObj([MTLStencilDescriptor new]),
m_objCContainer(nil),
m_frontFace(false)
{
    // Member initialization only
}

StencilDescriptor::StencilDescriptor(CPPMetalInternal::DepthStencilDescriptor objCContainer, bool frontFace) :
m_objCContainer(objCContainer),
m_frontFace(frontFace)
{
    if(frontFace)
    {
        m_objCObj = m_objCContainer.frontFaceStencil;
    }
    else
    {
        m_objCObj = m_objCContainer.backFaceStencil;
    }
}

StencilDescriptor::StencilDescriptor(const StencilDescriptor & rhs) :
m_objCObj([rhs.m_objCObj copyWithZone:nil]),
m_objCContainer(nil),
m_frontFace(false)
{
    // Member initialization only
}

StencilDescriptor & StencilDescriptor::operator=(const StencilDescriptor & rhs)
{

    // if(m_objCContainer == nil) return *this; <= Don't to check for nil since ObjC allows
    //    accessing a property of a nil object without consequence.

    if(m_objCContainer)
    {
        if(m_frontFace)
        {
            m_objCContainer.frontFaceStencil = rhs.m_objCObj;
            m_objCObj = m_objCContainer.frontFaceStencil;
        }
        else
        {
            m_objCContainer.backFaceStencil = rhs.m_objCObj;
            m_objCObj = m_objCContainer.backFaceStencil;
        }
    }
    else
    {
        m_objCObj = [rhs.m_objCObj copyWithZone:nil];
    }

    return *this;
}

StencilDescriptor::~StencilDescriptor()
{
    m_objCObj = nil;
}

void StencilDescriptor::reinitialize(CPPMetalInternal::DepthStencilDescriptor objCContainer)
{
    m_objCContainer = objCContainer;

    if(m_frontFace)
    {
        m_objCObj = objCContainer.frontFaceStencil;;
    }
    else
    {
        m_objCObj = objCContainer.backFaceStencil;;
    }
}

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(StencilDescriptor, CompareFunction, stencilCompareFunction);

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(StencilDescriptor, StencilOperation, stencilFailureOperation);

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(StencilDescriptor, StencilOperation, depthFailureOperation);

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(StencilDescriptor, StencilOperation, depthStencilPassOperation);

CPP_METAL_READWRITE_PROPERTY_IMPLEMENTATION(StencilDescriptor, uint32_t, readMask);

CPP_METAL_READWRITE_PROPERTY_IMPLEMENTATION(StencilDescriptor, uint32_t, writeMask);

#pragma mark - DepthStencilDescriptor

DepthStencilDescriptor::DepthStencilDescriptor()
: m_objCObj([MTLDepthStencilDescriptor new])
, frontFaceStencil(m_objCObj, true)
, backFaceStencil(m_objCObj, false)
{
}


DepthStencilDescriptor::DepthStencilDescriptor(const DepthStencilDescriptor & rhs)
: m_objCObj([rhs.m_objCObj copyWithZone:nil])
, frontFaceStencil(m_objCObj, true)
, backFaceStencil(m_objCObj, false)
{

}

DepthStencilDescriptor & DepthStencilDescriptor::operator=(const DepthStencilDescriptor & rhs)
{
    m_objCObj = [rhs.m_objCObj copyWithZone:nil];
    frontFaceStencil.reinitialize(m_objCObj);
    backFaceStencil.reinitialize(m_objCObj);

    return *this;
}

DepthStencilDescriptor::~DepthStencilDescriptor()
{
    m_objCObj = nil;
}

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(DepthStencilDescriptor, CompareFunction, depthCompareFunction);

CPP_METAL_READWRITE_BOOL_PROPERTY_IMPLEMENTATION(DepthStencilDescriptor, depthWriteEnabled, isDepthWriteEnabled);

CPP_METAL_READWRITE_LABEL_PROPERTY_IMPLEMENTATION(DepthStencilDescriptor);

#pragma mark - DepthStencilState

CPP_METAL_CONSTRUCTOR_IMPLEMENTATION(DepthStencilState);

CPP_METAL_NULL_REFERENCE_CONSTRUCTOR_IMPLEMENATATION(DepthStencilState);

CPP_METAL_COPY_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(DepthStencilState);

DepthStencilState::~DepthStencilState()
{
}

const char* DepthStencilState::label() const
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    return m_objCObj.label.UTF8String;
}

bool DepthStencilState::operator==(const DepthStencilState & rhs) const
{
    return [m_objCObj isEqual:rhs.objCObj()];
}

CPP_METAL_DEVICE_GETTER_IMPLEMENTATION(DepthStencilState);
