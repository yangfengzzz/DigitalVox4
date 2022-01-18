//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cpp_mtl_DeviceImplementation_h
#define cpp_mtl_DeviceImplementation_h

#include <Metal/Metal.h>
#include "cpp_mtl.h"
#include "cpp_mtl_allocator.h"
#include "cpp_mtl_render_command_encoder_dispatch_table.h"
#include "cpp_mtl_dispatch_table_cache.h"


using namespace MTL;

namespace cpp_mtl_internal
{

class DeviceInternals
{
public:

    DeviceInternals(Allocator *allocator);

    CPP_METAL_VIRTUAL ~DeviceInternals();

    Allocator & allocator();

    RenderCommandEncoderDispatchTable* getRenderCommandEncoderTable(id<MTLRenderCommandEncoder> objCObj);

private:

    Allocator *m_allocator;

    cpp_mtl_internal::DispatchTableCache<RenderCommandEncoderDispatchTable> m_renderCommandEncoderCache;

};

inline Allocator & DeviceInternals::allocator()
{
    return *m_allocator;
}


inline RenderCommandEncoderDispatchTable* DeviceInternals::getRenderCommandEncoderTable(id<MTLRenderCommandEncoder> objCObj)
{
    return m_renderCommandEncoderCache.getTable(objCObj);
}


} // namespace cpp_mtl_internal

#endif // cpp_mtl_DeviceImplementation_h
