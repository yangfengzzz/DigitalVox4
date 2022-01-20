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
    MetalApplication();
    
    virtual ~MetalApplication();
    
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
        
    void inputEvent(const InputEvent &inputEvent) override;
    
    void finish() override;
    
private:
    MTL::Library makeShaderLibrary();
    
protected:
    /**
     * @brief The Metal device
     */
    std::unique_ptr<MTL::Device> _device{nullptr};
    
    /**
     * @brief The Metal command queue
     */
    MTL::CommandQueue _commandQueue;
    
    /**
     * @brief The Metal shader library
     */
    MTL::Library _library;

    /**
     * @brief View used for rendering, it is responsible for managing the frames and their underlying images
     */
    std::unique_ptr<View> _renderView{nullptr};
};

}        // namespace vox


#endif /* metal_application_hpp */
