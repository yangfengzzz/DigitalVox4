//
//  compose_subpass.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#ifndef compose_subpass_hpp
#define compose_subpass_hpp

#include "../subpass.h"

namespace vox {
class ComposeSubpass: public Subpass {
public:
    ComposeSubpass(MTL::RenderPassDescriptor* desc,
                   Scene* scene,
                   MTL::Library& shaderLibrary,
                   MTL::Device& m_device,
                   MTL::PixelFormat colorPixelFormat,
                   MTL::Buffer &m_quadVertexBuffer,
                   MTL::RenderPassDescriptor* gbuffer_desc);
    
    void draw(MTL::RenderCommandEncoder& commandEncoder) override;
    
private:
    MTL::RenderPipelineState m_directionalLightPipelineState;
    MTL::DepthStencilState m_directionLightDepthStencilState;
    
    MTL::Buffer &m_quadVertexBuffer;
    MTL::RenderPassDescriptor* gbuffer_desc;
};

}

#endif /* compose_subpass_hpp */
