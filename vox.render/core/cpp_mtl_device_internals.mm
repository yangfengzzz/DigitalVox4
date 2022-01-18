//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cpp_mtl_device_internals.h"
#include "cpp_mtl_pixel_format.h"
#include "cpp_mtl_internal_macros.h"
#include <Metal/Metal.h>

using namespace MTL;
using namespace cpp_mtl_Internal;

DeviceInternals::DeviceInternals(Allocator *allocator)
: m_allocator(allocator), m_renderCommandEncoderCache(*allocator) {
    
}

DeviceInternals::~DeviceInternals() {
    
}
