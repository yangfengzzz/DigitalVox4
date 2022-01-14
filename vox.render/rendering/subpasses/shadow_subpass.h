//
//  shadow_subpass.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#ifndef shadow_subpass_hpp
#define shadow_subpass_hpp

#include "../subpass.h"
#include "graphics/mesh.h"
#include <vector>

namespace vox {
class ShadowSubpass: public Subpass {
public:
    ShadowSubpass(MTL::RenderPassDescriptor* desc,
                  sg::Scene* scene,
                  MTL::Library& shaderLibrary,
                  MTL::Device& m_device,
                  std::vector<Mesh> *m_meshes);

    void draw(MTL::RenderCommandEncoder& commandEncoder) override;
        
private:
    void drawMeshes(MTL::RenderCommandEncoder &renderEncoder);

    MTL::RenderPipelineState m_shadowGenPipelineState;
    MTL::DepthStencilState m_shadowDepthStencilState;
    
    std::vector<Mesh> *m_meshes;
};

}

#endif /* shadow_subpass_hpp */
