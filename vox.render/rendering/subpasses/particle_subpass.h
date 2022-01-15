//
//  particle_subpass.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/15.
//

#ifndef particle_subpass_hpp
#define particle_subpass_hpp

#include "../subpass.h"

namespace vox {
class ParticleSubpass: public Subpass {
public:
    ParticleSubpass(MTL::RenderPassDescriptor* desc,
                    sg::Scene* scene,
                    MTL::Library& shaderLibrary,
                    MTL::Device& m_device,
                    MTL::PixelFormat colorPixelFormat,
                    MTL::Buffer &m_fairy,
                    MTL::Texture &m_fairyMap,
                    const uint32_t NumLights,
                    const uint32_t NumFairyVertices);
    
    void draw(MTL::RenderCommandEncoder& commandEncoder) override;
    
private:
    MTL::DepthStencilState m_dontWriteDepthStencilState;
    MTL::RenderPipelineState m_fairyPipelineState;
    
    // Mesh buffer for fairies
    MTL::Buffer &m_fairy;
    // Texture to create smooth round particles
    MTL::Texture &m_fairyMap;
    
    const uint32_t NumLights;
    const uint32_t NumFairyVertices;
};

}

#endif /* particle_subpass_hpp */
