//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef sky_h
#define sky_h

#include "scene_forward.h"

namespace vox {
/**
 * Sky.
 */
struct Sky {
    /** Material of the sky. */
    MaterialPtr material{nullptr};
    /** Mesh of the sky. */
    MeshPtr mesh{nullptr};
};

}
#endif /* sky_h */
