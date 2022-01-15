//
//  subpass.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#include "subpass.h"

namespace vox {
Subpass::Subpass(MTL::RenderPassDescriptor* desc,
                 MTL::Device& m_device,
                 Scene* scene):
m_device(m_device),
scene(scene) {
}

}
