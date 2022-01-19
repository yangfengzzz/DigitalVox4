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
    
private:
    void drawPointLightMask(MTL::RenderCommandEncoder &commandEncoder);
    
    void drawPointLights(MTL::RenderCommandEncoder &commandEncoder);
    
    MTL::RenderPipelineState _lightMaskPipelineState;
    MTL::DepthStencilState _lightMaskDepthStencilState;
    
    MTL::RenderPipelineState _lightPipelineState;
    MTL::DepthStencilState _pointLightDepthStencilState;
    
    MeshPtr _icosahedronMesh;
    uint32_t _numLights;
    MTL::Texture &_albedoTexture;
    MTL::Texture &_normalTexture;
    MTL::Texture &_depthTexture;
};

}

#endif /* point_light_subpass_hpp */
