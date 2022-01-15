//
//  point_light_subpass.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/15.
//

#include "point_light_subpass.h"

namespace vox {
PointLightSubpass::PointLightSubpass(MTL::RenderPassDescriptor* desc,
                                     sg::Scene* scene,
                                     MTL::Library& shaderLibrary,
                                     MTL::Device& m_device):
Subpass(desc, scene) {
    
}

void PointLightSubpass::draw(MTL::RenderCommandEncoder& commandEncoder) {
    drawPointLightMask(commandEncoder);
    drawPointLights(commandEncoder);
}

void PointLightSubpass::drawPointLightMask(MTL::RenderCommandEncoder &commandEncoder) {
    
}

void PointLightSubpass::drawPointLights(MTL::RenderCommandEncoder &commandEncoder) {
    
}

}
