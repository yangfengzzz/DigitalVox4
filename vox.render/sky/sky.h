//
//  sky.h
//  DigitalVox4
//
//  Created by 杨丰 on 2022/1/15.
//

#ifndef sky_h
#define sky_h

#include "scene_forward.h"

namespace vox {
/**
 * Sky.
 */
struct Sky {
    /** Material of the sky. */
    sg::MaterialPtr material{nullptr};
    /** Mesh of the sky. */
    sg::MeshPtr mesh{nullptr};
};

}
#endif /* sky_h */
