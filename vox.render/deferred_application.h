//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef deferred_hpp
#define deferred_hpp

#include "metal_application.h"

#include "config.h"

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
    bool prepare(Engine &engine) override;
    
    /**
     * @brief Main loop sample events
     */
    void update(float delta_time) override;
    
    bool resize(uint32_t win_width, uint32_t win_height,
                uint32_t fb_width, uint32_t fb_height) override;
    
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
