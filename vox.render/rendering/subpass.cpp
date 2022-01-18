//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "subpass.h"

namespace vox {
Subpass::Subpass(MTL::RenderPassDescriptor* desc,
                 MTL::Device* device,
                 Scene* scene,
                 Camera* camera):
_device(device),
_scene(scene),
_camera(camera) {
}

}
