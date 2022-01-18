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
    ComposeSubpass(MTL::RenderPassDescriptor* desc,
                   MTL::Device* device,
                   Scene* scene,
                   Camera* camera,
                   MTL::Library& shaderLibrary,
                   MTL::PixelFormat colorPixelFormat,
                   MTL::RenderPassDescriptor* gbufferDesc);
    
    void draw(MTL::RenderCommandEncoder& commandEncoder) override;
    
private:
    MTL::RenderPipelineState _directionalLightPipelineState;
    MTL::DepthStencilState _directionLightDepthStencilState;
    
    MTL::Buffer _quadVertexBuffer;
    MTL::RenderPassDescriptor* _gbufferDesc;
};

}

#endif /* compose_subpass_hpp */
