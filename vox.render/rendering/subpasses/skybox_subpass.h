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
                  sg::Scene* scene,
                  MTL::Library& shaderLibrary,
                  MTL::Device& m_device,
                  MTL::PixelFormat colorPixelFormat,
                  Mesh& m_skyMesh,
                  MTL::VertexDescriptor& m_skyVertexDescriptor,
                  MTL::Texture& m_skyMap);
    
    void draw(MTL::RenderCommandEncoder& commandEncoder) override;
    
private:
    MTL::DepthStencilState m_dontWriteDepthStencilState;
    MTL::RenderPipelineState m_skyboxPipelineState;
    
    Mesh& m_skyMesh;
    MTL::Texture& m_skyMap;
};

}

#endif /* skybox_subpass_hpp */
