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
class ForwardApplication: public MetalApplication {
public:
    ForwardApplication() = default;
    
    virtual ~ForwardApplication();
    
    /**
     * @brief Additional sample initialization
     */
    virtual bool prepare(Engine &engine) override;
    
    /**
     * @brief Main loop sample events
     */
    virtual void update(float delta_time) override;
        
    virtual void framebufferResize(uint32_t width, uint32_t height) override;
    
    virtual void loadScene(uint32_t width, uint32_t height) = 0;
    
protected:
    Camera* _mainCamera{nullptr};
    
private:
    MTL::RenderPassDescriptor _finalRenderPassDescriptor;
    std::unique_ptr<RenderPass> _finalRenderPass{nullptr};
};

}

#endif /* forward_hpp */
