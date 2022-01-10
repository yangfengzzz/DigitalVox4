/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Implementation of C++ command queue class wrapper
*/

#include "CPPMetalCommandQueue.hpp"
#include "CPPMetalCommandBuffer.hpp"
#include "CPPMetalDevice.hpp"
#include "CPPMetalInternalMacros.h"
#include "CPPMetalDeviceInternals.h"

#include <Metal/Metal.h>

using namespace MTL;

CPP_METAL_CONSTRUCTOR_IMPLEMENTATION(CommandQueue);

CPP_METAL_NULL_REFERENCE_CONSTRUCTOR_IMPLEMENATATION(CommandQueue);

CPP_METAL_COPY_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(CommandQueue);

CommandQueue::~CommandQueue()
{
    m_objCObj = nil;
}

CommandBuffer CommandQueue::commandBuffer()
{
    const id<MTLCommandBuffer> objCObj = [m_objCObj commandBuffer];

    return CommandBuffer(objCObj, *m_device);
}

CPP_METAL_READWRITE_LABEL_PROPERTY_IMPLEMENTATION(CommandQueue);

CPP_METAL_DEVICE_GETTER_IMPLEMENTATION(CommandQueue);
