//
//  deferred.hpp
//  apps
//
//  Created by 杨丰 on 2022/1/14.
//

#ifndef deferred_hpp
#define deferred_hpp

#include "metal_application.h"

#include "config.h"
#include "mesh.h"
#include "math_utilities.h"

namespace vox {
// The max number of command buffers in flight
static const uint8_t MaxFramesInFlight = 3;

class DeferredApplication : public MetalApplication {
public:
    DeferredApplication() = default;
    
    virtual ~DeferredApplication();
    
    /**
     * @brief Additional sample initialization
     */
    virtual bool prepare(Engine &engine) override;
    
    /**
     * @brief Main loop sample events
     */
    virtual void update(float delta_time) override;
    
    virtual void framebufferResize(uint32_t width, uint32_t height) override;
    
    virtual void loadScene() = 0;
    
protected:
    /**
     * @brief Holds all scene information
     */
    std::unique_ptr<Scene> _scene{nullptr};
    
private:    
    dispatch_semaphore_t m_inFlightSemaphore;
    MTL::CommandBufferHandler *m_completedHandler{nullptr};
    
    MTL::RenderPassDescriptor m_shadowRenderPassDescriptor;
    std::unique_ptr<RenderPass> m_shadowRenderPass{nullptr};
    MTL::Texture m_shadowMap;
    
    MTL::RenderPassDescriptor m_GBufferRenderPassDescriptor;
    std::unique_ptr<RenderPass> m_GBufferRenderPass{nullptr};
    MTL::PixelFormat m_albedo_specular_GBufferFormat;
    MTL::PixelFormat m_normal_shadow_GBufferFormat;
    MTL::PixelFormat m_depth_GBufferFormat;
    MTL::Texture m_albedo_specular_GBuffer;
    MTL::Texture m_normal_shadow_GBuffer;
    MTL::Texture m_depth_GBuffer;
    
    MTL::RenderPassDescriptor m_finalRenderPassDescriptor;
    std::unique_ptr<RenderPass> m_finalRenderPass{nullptr};
};

}

#endif /* deferred_hpp */
