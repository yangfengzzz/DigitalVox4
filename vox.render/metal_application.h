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
/**
 * @mainpage Overview of the framework
 *
 * @section initialization Initialization
 *
 * @subsection platform_init Platform initialization
 * The lifecycle of a Metal sample starts by instantiating the correct Platform
 * (e.g. WindowsPlatform) and then calling initialize() on it, which sets up
 * the windowing system and logging. Then it calls the parent Platform::initialize(),
 * which takes ownership of the active application. It's the platforms responsibility
 * to then call MetalSample::prepare() to prepare the metal sample when it is ready.
 *
 * @subsection sample_init Sample initialization
 * The preparation step is divided in two steps, one in MetalSample and the other in the
 * specific sample, such as SurfaceRotation.
 * MetalSample::prepare() contains functions that do not require customization,
 * including creating a Metal instance, the surface and getting physical devices.
 * The prepare() function for the specific sample completes the initialization, including:
 * - setting enabled Stats
 * - creating the Device
 * - creating the Swapchain
 * - creating the RenderContext (or child class)
 * - preparing the RenderContext
 * - loading the Scene
 * - creating the RenderPipeline with ShaderModule (s)
 * - creating the Camera
 * - creating the Gui
 *
 * @section frame_rendering Frame rendering
 *
 * @subsection update Update function
 * Rendering happens in the update() function. Each sample can override it, e.g.
 * to recreate the Swapchain in SwapchainImages when required by user input.
 * Typically a sample will then call MetalSample::update().
 *
 * @subsection rendering Rendering
 * A series of steps are performed, some of which can be customized (it will be
 * highlighted when that's the case):
 *
 * - calling Script::update() for all Script (s)
 * - beginning a frame in RenderContext (does the necessary waiting on fences and
 *   acquires an core::Image)
 * - requesting a CommandBuffer
 * - updating Stats and Gui
 * - getting an active RenderTarget constructed by the factory function of the RenderFrame
 * - setting up barriers for color and depth, note that these are only for the default RenderTarget
 * - calling MetalSample::draw_swapchain_renderpass (see below)
 * - setting up a barrier for the Swapchain transition to present
 * - submitting the CommandBuffer and end the Frame (present)
 *
 * @subsection draw_swapchain Draw swapchain renderpass
 * The function starts and ends a RenderPass which includes setting up viewport, scissors,
 * blend state (etc.) and calling draw_scene.
 * Note that RenderPipeline::draw is not virtual in RenderPipeline, but internally it calls
 * Subpass::draw for each Subpass, which is virtual and can be customized.
 *
 * @section framework_classes Main framework classes
 *
 * - RenderContext
 * - RenderFrame
 * - RenderTarget
 * - RenderPipeline
 * - ShaderModule
 * - ResourceCache
 * - BufferPool
 * - Core classes: Classes in vox::core wrap Metal objects for indexing and hashing.
 */

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
    
    /**
     * @brief Loads the scene
     *
     * @param path The path of the glTF file
     */
    void loadScene(const std::string &path);
    
    bool hasScene();
    
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
     * @brief Context used for rendering, it is responsible for managing the frames and their underlying images
     */
    std::unique_ptr<View> _renderContext{nullptr};

    /**
     * @brief Pipeline used for rendering, it should be set up by the concrete sample
     */
    std::unique_ptr<RenderPass> _renderPipeline{nullptr};

    /**
     * @brief Holds all scene information
     */
    std::unique_ptr<Scene> _scene{nullptr};
    
    /**
     * @brief Update scene
     * @param deltaTime delta_time
     */
    void updateScene(float deltaTime);
    
    /**
     * @brief Update counter values
     * @param deltaTime delta_time
     */
    void updateStats(float deltaTime);
    
    /**
     * @brief Update GUI
     * @param deltaTime delta_time
     */
    void updateGui(float deltaTime);
};

}        // namespace vox


#endif /* metal_application_hpp */
