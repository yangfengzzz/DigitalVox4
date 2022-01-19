//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef metal_application_hpp
#define metal_application_hpp

#include "application.h"
#include "core/cpp_mtl.h"
#include "rendering/render_pass.h"
#include "scene.h"

namespace vox {
class MetalApplication : public Application {
public:
    MetalApplication() = default;
    
    virtual ~MetalApplication();
    
    /**
     * @brief Additional sample initialization
     */
    virtual bool prepare(Engine &engine) override;
    
    /**
     * @brief Main loop sample events
     */
    virtual void update(float delta_time) override;
    
    virtual bool resize(const uint32_t width, const uint32_t height) override;
    
    virtual void framebufferResize(uint32_t width, uint32_t height) override;
    
    virtual void inputEvent(const InputEvent &inputEvent) override;
    
    virtual void finish() override;
    
private:
    MTL::Library makeShaderLibrary();
    
protected:
    /**
     * @brief The Metal device
     */
    std::unique_ptr<MTL::Device> _device{nullptr};
    
    MTL::CommandQueue _commandQueue;
    
    MTL::Library _library;

    /**
     * @brief View used for rendering, it is responsible for managing the frames and their underlying images
     */
    std::unique_ptr<View> _renderView{nullptr};

    /**
     * @brief Pipeline used for rendering, it should be set up by the concrete sample
     */
    std::unique_ptr<RenderPass> _renderPipeline{nullptr};

    /**
     * @brief Holds all scene information
     */
    std::unique_ptr<Scene> _scene{nullptr};
};

}        // namespace vox


#endif /* metal_application_hpp */
