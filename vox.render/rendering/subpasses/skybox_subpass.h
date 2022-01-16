//
//  skybox_subpass.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/15.
//

#ifndef skybox_subpass_hpp
#define skybox_subpass_hpp

#include "../subpass.h"
#include "graphics/mesh.h"

namespace vox {
class SkyboxSubpass: public Subpass {
public:
    SkyboxSubpass(MTL::RenderPassDescriptor* desc,
                  Scene* scene,
                  Camera* camera,
                  MTL::Library& shaderLibrary,
                  MTL::Device& m_device,
                  MTL::PixelFormat colorPixelFormat);
    
    void draw(MTL::RenderCommandEncoder& commandEncoder) override;
    
private:
    MTL::DepthStencilState m_dontWriteDepthStencilState;
    MTL::RenderPipelineState m_skyboxPipelineState;
};

}

#endif /* skybox_subpass_hpp */
