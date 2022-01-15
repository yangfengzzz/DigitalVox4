//
//  point_light_subpass.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/15.
//

#ifndef point_light_subpass_hpp
#define point_light_subpass_hpp

#include "../subpass.h"

namespace vox {
class PointLightSubpass: public Subpass {
public:
    PointLightSubpass(MTL::RenderPassDescriptor* desc,
                      sg::Scene* scene,
                      MTL::Library& shaderLibrary,
                      MTL::Device& m_device);
    
    void draw(MTL::RenderCommandEncoder& commandEncoder) override;
    
private:
    void drawPointLightMask(MTL::RenderCommandEncoder &commandEncoder);
    
    void drawPointLights(MTL::RenderCommandEncoder &commandEncoder);
};

}

#endif /* point_light_subpass_hpp */
