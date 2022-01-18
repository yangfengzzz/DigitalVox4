//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.
#ifndef forward_subpass_hpp
#define forward_subpass_hpp

#include "../subpass.h"

namespace vox {
class ForwardSubpass: public Subpass {
public:
    ForwardSubpass(MTL::RenderPassDescriptor* desc,
                   MTL::Device* device,
                   Scene* scene,
                   Camera* camera,
                   MTL::Library& shaderLibrary,
                   MTL::PixelFormat colorPixelFormat);
    
    void draw(MTL::RenderCommandEncoder& commandEncoder) override;
    
private:
    void drawMeshes(MTL::RenderCommandEncoder &renderEncoder);
    
    MTL::RenderPipelineDescriptor _forwardPipelineDescriptor;
    MTL::DepthStencilState _forwardDepthStencilState;
};

}

#endif /* forward_subpass_hpp */
