/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Implementation of C++ Metal command encoder class wrapper
*/

#include "CPPMetalCommandEncoder.hpp"
#include "CPPMetalDevice.hpp"
#include <Metal/Metal.h>
#include "CPPMetalInternalMacros.h"

using namespace MTL;

CPP_METAL_CONSTRUCTOR_IMPLEMENTATION(CommandEncoder);

CPP_METAL_COPY_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(CommandEncoder);

CommandEncoder::~CommandEncoder()
{
    m_objCObj = nil;
}

CPP_METAL_READWRITE_LABEL_PROPERTY_IMPLEMENTATION(CommandEncoder);

CPP_METAL_DEVICE_GETTER_IMPLEMENTATION(CommandEncoder);

void CommandEncoder::pushDebugGroup(const CFStringRef string)
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    [m_objCObj pushDebugGroup:(__bridge NSString*)string];
}

void CommandEncoder::popDebugGroup()
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    [m_objCObj popDebugGroup];
}

void CommandEncoder::endEncoding()
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    [m_objCObj endEncoding];
}
