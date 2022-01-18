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
    ParticleSubpass(MTL::RenderPassDescriptor* desc,
                    MTL::Device* device,
                    Scene* scene,
                    Camera* camera,
                    MTL::Library& shaderLibrary,
                    MTL::PixelFormat colorPixelFormat,
                    MTL::Buffer &fairy,
                    MTL::Texture &fairyMap,
                    const uint32_t numLights,
                    const uint32_t numFairyVertices);
    
    void draw(MTL::RenderCommandEncoder& commandEncoder) override;
    
private:
    MTL::DepthStencilState _dontWriteDepthStencilState;
    MTL::RenderPipelineState _fairyPipelineState;
    
    // Mesh buffer for fairies
    MTL::Buffer &_fairy;
    // Texture to create smooth round particles
    MTL::Texture &_fairyMap;
    
    const uint32_t _numLights;
    const uint32_t _numFairyVertices;
};

}

#endif /* particle_subpass_hpp */
