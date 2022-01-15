//
//  point_light_subpass.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/15.
//

#ifndef point_light_subpass_hpp
#define point_light_subpass_hpp

#include "../subpass.h"
#include "graphics/mesh.h"

namespace vox {
class PointLightSubpass: public Subpass {
public:
    PointLightSubpass(MTL::RenderPassDescriptor* desc,
                      sg::Scene* scene,
                      MTL::Library& shaderLibrary,
                      MTL::Device& m_device,
                      MTL::PixelFormat colorPixelFormat,
                      Mesh& m_icosahedronMesh,
                      MTL::RenderPassDescriptor* gbuffer_desc,
                      const uint32_t NumLights);
    
    void draw(MTL::RenderCommandEncoder& commandEncoder) override;
    
private:
    void drawPointLightMask(MTL::RenderCommandEncoder &commandEncoder);
    
    void drawPointLights(MTL::RenderCommandEncoder &commandEncoder);
    
    MTL::RenderPipelineState m_lightMaskPipelineState;
    MTL::DepthStencilState m_lightMaskDepthStencilState;
    
    MTL::RenderPipelineState m_lightPipelineState;
    MTL::DepthStencilState m_pointLightDepthStencilState;
    
    Mesh& m_icosahedronMesh;
    MTL::RenderPassDescriptor* gbuffer_desc;
    const uint32_t NumLights;
};

}

#endif /* point_light_subpass_hpp */
