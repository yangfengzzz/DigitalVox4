//
//  shadow_subpass.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#ifndef shadow_subpass_hpp
#define shadow_subpass_hpp

#include "../subpass.h"

namespace vox {
class ShadowSubpass: public Subpass {
public:
    ShadowSubpass(MTL::RenderPassDescriptor* desc,
                  MTL::Library& shaderLibrary,
                  MTL::Device& m_device);

    void draw(MTL::RenderCommandEncoder commandEncoder) override;
    
private:
    MTL::RenderPipelineState m_shadowGenPipelineState;
    MTL::DepthStencilState m_shadowDepthStencilState;
    MTL::Texture m_shadowMap;
};

}

#endif /* shadow_subpass_hpp */
