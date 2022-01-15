//
//  forward_subpass.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/15.
//

#include "forward_subpass.h"

namespace vox {
ForwardSubpass::ForwardSubpass(MTL::RenderPassDescriptor* desc,
                               Scene* scene,
                               MTL::Library& shaderLibrary,
                               MTL::Device& m_device,
                               MTL::PixelFormat colorPixelFormat):
Subpass(desc, m_device, scene) {
}

void ForwardSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    
}

}
