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
    PointLightSubpass(MTL::RenderPassDescriptor* desc,
                      MTL::Device* device,
                      Scene* scene,
                      Camera* camera,
                      MTL::Library& shaderLibrary,
                      MTL::PixelFormat colorPixelFormat,
                      MeshPtr icosahedronMesh,
                      MTL::RenderPassDescriptor* gbufferDesc,
                      const uint32_t numLights);
    
    void draw(MTL::RenderCommandEncoder& commandEncoder) override;
    
private:
    void drawPointLightMask(MTL::RenderCommandEncoder &commandEncoder);
    
    void drawPointLights(MTL::RenderCommandEncoder &commandEncoder);
    
    MTL::RenderPipelineState _lightMaskPipelineState;
    MTL::DepthStencilState _lightMaskDepthStencilState;
    
    MTL::RenderPipelineState _lightPipelineState;
    MTL::DepthStencilState _pointLightDepthStencilState;
    
    MeshPtr _icosahedronMesh;
    MTL::RenderPassDescriptor* _gbufferDesc;
    uint32_t _numLights;
};

}

#endif /* point_light_subpass_hpp */
