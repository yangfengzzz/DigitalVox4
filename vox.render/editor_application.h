//
//  editor_application.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/20.
//

#ifndef editor_application_hpp
#define editor_application_hpp

#include "metal_application.h"

namespace vox {
class EditorApplication: public MetalApplication {
public:
    EditorApplication() = default;
    
    virtual ~EditorApplication();
    
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
    
    virtual void loadScene(uint32_t width, uint32_t height) = 0;
    
public:
    virtual void pickFunctor(Renderer *renderer, MeshPtr mesh) {};
    
    /**
     * Pick the object at the screen coordinate position.
     * @param offsetX Relative X coordinate of the canvas
     * @param offsetY Relative Y coordinate of the canvas
     */
    void pick(float offsetX, float offsetY);
    
protected:
    Camera* _mainCamera{nullptr};
    
    /**
     * @brief Holds all scene information
     */
    std::unique_ptr<Scene> _scene{nullptr};
    
private:
    MTL::RenderPassDescriptor _renderPassDescriptor;
    
    /**
     * @brief Pipeline used for rendering, it should be set up by the concrete sample
     */
    std::unique_ptr<RenderPass> _renderPass{nullptr};
    
    bool _needPick;
    Vector2F _pickPos;
    MTL::PixelFormat _colorPickerFormat;
    MTL::Texture _colorPickerTexture;
    MTL::RenderPassDescriptor _colorPickerPassDescriptor;
    std::unique_ptr<RenderPass> _colorPickerRenderPass{nullptr};
    
    std::array<uint8_t, 4> _readColorFromRenderTarget();
};

}

#endif /* editor_application_hpp */
