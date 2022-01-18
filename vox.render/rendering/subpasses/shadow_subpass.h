//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef shadow_subpass_hpp
#define shadow_subpass_hpp

#include "../subpass.h"
#include "graphics/mesh.h"
#include <vector>

namespace vox {
class ShadowSubpass: public Subpass {
public:
    ShadowSubpass(MTL::RenderPassDescriptor* desc,
                  MTL::Device* device,
                  Scene* scene,
                  Camera* camera,
                  MTL::Library& shaderLibrary);

    void draw(MTL::RenderCommandEncoder& commandEncoder) override;
        
private:
    void drawMeshes(MTL::RenderCommandEncoder &renderEncoder);

    MTL::RenderPipelineState _shadowGenPipelineState;
    MTL::DepthStencilState _shadowDepthStencilState;
};

}

#endif /* shadow_subpass_hpp */
