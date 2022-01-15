//
//  particle_subpass.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/15.
//

#include "particle_subpass.h"

namespace vox {
ParticleSubpass::ParticleSubpass(MTL::RenderPassDescriptor* desc,
                                 sg::Scene* scene,
                                 MTL::Library& shaderLibrary,
                                 MTL::Device& m_device,
                                 MTL::PixelFormat colorPixelFormat):
Subpass(desc, scene) {
    
}

void ParticleSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    
}

}
