//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_subpass_hpp
#define particle_subpass_hpp

#include "../subpass.h"

namespace vox {
class ParticleSubpass: public Subpass {
public:
    ParticleSubpass(View* view,
                    Scene* scene,
                    Camera* camera,
                    MTL::Buffer &fairy,
                    MTL::Texture &fairyMap,
                    const uint32_t numLights,
                    const uint32_t numFairyVertices);
    
    void prepare() override;

    void draw(MTL::RenderCommandEncoder& commandEncoder) override;
    
private:
    MTL::RenderPipelineDescriptor _fairyPipelineDescriptor;
    MTL::DepthStencilState _dontWriteDepthStencilState;
    
    // Mesh buffer for fairies
    MTL::Buffer &_fairy;
    // Texture to create smooth round particles
    MTL::Texture &_fairyMap;
    
    const uint32_t _numLights;
    const uint32_t _numFairyVertices;
};

}

#endif /* particle_subpass_hpp */
