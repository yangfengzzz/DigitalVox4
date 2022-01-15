//
//  forward_subpass.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/15.
//

#ifndef forward_subpass_hpp
#define forward_subpass_hpp

#include "../subpass.h"

namespace vox {
class ForwardSubpass: public Subpass {
public:
    ForwardSubpass(MTL::RenderPassDescriptor* desc,
                   Scene* scene,
                   MTL::Library& shaderLibrary,
                   MTL::Device& m_device,
                   MTL::PixelFormat colorPixelFormat);
    
    void draw(MTL::RenderCommandEncoder& commandEncoder) override;
    
private:
    MTL::RenderPipelineState m_directionalLightPipelineState;
    MTL::DepthStencilState m_directionLightDepthStencilState;
};

}

#endif /* forward_subpass_hpp */
