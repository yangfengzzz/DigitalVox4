//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef compose_subpass_hpp
#define compose_subpass_hpp

#include "../subpass.h"

namespace vox {
class ComposeSubpass: public Subpass {
public:
    ComposeSubpass(View* view,
                   Scene* scene,
                   Camera* camera);
    
    void prepare() override;

    void draw(MTL::RenderCommandEncoder& commandEncoder) override;
    
    void setRenderPass(RenderPass* pass) override;
    
    static std::string dependedPassName();
    
private:
    MTL::RenderPipelineDescriptor _directionalLightPipelineDescriptor;
    MTL::DepthStencilState _directionLightDepthStencilState;
    
    MTL::Buffer _quadVertexBuffer;
    MTL::Texture *_albedoTexture{nullptr};
    MTL::Texture *_normalTexture{nullptr};
    MTL::Texture *_depthTexture{nullptr};
};

}

#endif /* compose_subpass_hpp */
