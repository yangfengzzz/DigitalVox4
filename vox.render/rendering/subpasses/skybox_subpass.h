//
//  skybox_subpass.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/15.
//

#ifndef skybox_subpass_hpp
#define skybox_subpass_hpp

#include "../subpass.h"

namespace vox {
class SkyboxSubpass: public Subpass {
public:
    SkyboxSubpass(MTL::RenderPassDescriptor* desc,
                      sg::Scene* scene,
                      MTL::Library& shaderLibrary,
                      MTL::Device& m_device);
    
    void draw(MTL::RenderCommandEncoder& commandEncoder) override;
};

}

#endif /* skybox_subpass_hpp */
