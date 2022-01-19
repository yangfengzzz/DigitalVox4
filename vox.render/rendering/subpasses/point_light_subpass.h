//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef point_light_subpass_hpp
#define point_light_subpass_hpp

#include "../subpass.h"
#include "graphics/mesh.h"

namespace vox {
class PointLightSubpass: public Subpass {
public:
    PointLightSubpass(View* view,
                      Scene* scene,
                      Camera* camera,
                      MeshPtr icosahedronMesh,
                      const uint32_t numLights);
    
    void prepare() override;

    void draw(MTL::RenderCommandEncoder& commandEncoder) override;
    
    void setRenderPass(RenderPass* pass) override;
    
    static std::string dependedPassName();
    
private:
    void drawPointLightMask(MTL::RenderCommandEncoder &commandEncoder);
    
    void drawPointLights(MTL::RenderCommandEncoder &commandEncoder);
    
    MTL::RenderPipelineDescriptor _lightMaskPipelineDescriptor;
    MTL::DepthStencilState _lightMaskDepthStencilState;
    
    MTL::RenderPipelineDescriptor _lightPipelineDescriptor;
    MTL::DepthStencilState _pointLightDepthStencilState;
    
    MeshPtr _icosahedronMesh;
    uint32_t _numLights;
    MTL::Texture *_albedoTexture{nullptr};
    MTL::Texture *_normalTexture{nullptr};
    MTL::Texture *_depthTexture{nullptr};
};

}

#endif /* point_light_subpass_hpp */
