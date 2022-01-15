//
//  skybox_subpass.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/15.
//

#include "skybox_subpass.h"

namespace vox {
SkyboxSubpass::SkyboxSubpass(MTL::RenderPassDescriptor* desc,
                             sg::Scene* scene,
                             MTL::Library& shaderLibrary,
                             MTL::Device& m_device):
Subpass(desc, scene) {
    
}

void SkyboxSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    
}

}
