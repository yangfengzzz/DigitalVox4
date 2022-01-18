//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef skybox_subpass_hpp
#define skybox_subpass_hpp

#include "../subpass.h"
#include "graphics/mesh.h"

namespace vox {
class SkyboxSubpass: public Subpass {
public:
    SkyboxSubpass(MTL::RenderPassDescriptor* desc,
                  MTL::Device* device,
                  Scene* scene,
                  Camera* camera,
                  MTL::Library& shaderLibrary,
                  MTL::PixelFormat colorPixelFormat);
    
    void draw(MTL::RenderCommandEncoder& commandEncoder) override;
    
private:
    MTL::DepthStencilState _dontWriteDepthStencilState;
    MTL::RenderPipelineState _skyboxPipelineState;
};

}

#endif /* skybox_subpass_hpp */
