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
                    MTL::PixelFormat colorPixelFormat);
    
    void draw(MTL::RenderCommandEncoder& commandEncoder) override;
    
private:
    
};

}

#endif /* particle_subpass_hpp */
