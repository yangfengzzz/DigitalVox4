//
//  forward.hpp
//  apps
//
//  Created by 杨丰 on 2022/1/16.
//

#ifndef forward_hpp
#define forward_hpp

#include "metal_application.h"

namespace vox {
class Forward: public MetalApplication {
public:
    Forward() = default;
    
    virtual ~Forward();
    
    /**
     * @brief Additional sample initialization
     */
    virtual bool prepare(Engine &engine) override;
    
    /**
     * @brief Main loop sample events
     */
    virtual void update(float delta_time) override;
    
    virtual void framebuffer_resize(uint32_t width, uint32_t height) override;
    
private:
    void loadScene();
    // Vertex descriptor for models loaded with MetalKit
    MTL::VertexDescriptor m_defaultVertexDescriptor;
    
    MTL::RenderPassDescriptor m_finalRenderPassDescriptor;
    std::unique_ptr<RenderPass> m_finalRenderPass{nullptr};
};

}

#endif /* forward_hpp */
