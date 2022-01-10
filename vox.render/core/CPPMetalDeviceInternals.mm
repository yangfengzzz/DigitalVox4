/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Implementation code for C++ Metal wrapper device intenrals class
*/

#include "CPPMetalDeviceInternals.h"
#include "CPPMetalPixelFormat.hpp"
#include "CPPMetalInternalMacros.h"
#include <Metal/Metal.h>


using namespace MTL;
using namespace CPPMetalInternal;


DeviceInternals::DeviceInternals(Allocator *allocator)
: m_allocator(allocator)
, m_renderCommandEncoderCache(*allocator)
{

}

DeviceInternals::~DeviceInternals()
{

}
