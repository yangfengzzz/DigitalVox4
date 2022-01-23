//
//  editor_application.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/20.
//

#ifndef editor_application_hpp
#define editor_application_hpp

#include "forward_application.h"

namespace vox {
class ColorPickerSubpass;

class EditorApplication: public ForwardApplication {
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
    
public:
    virtual void pickFunctor(Renderer *renderer, MeshPtr mesh) {};
    
    /**
     * Pick the object at the screen coordinate position.
     * @param offsetX Relative X coordinate of the canvas
     * @param offsetY Relative Y coordinate of the canvas
     */
    void pick(float offsetX, float offsetY);
    
private:
    bool _needPick;
    Vector2F _pickPos;
    MTL::PixelFormat _colorPickerFormat;
    MTL::Texture _colorPickerTexture;
    
    MTL::RenderPassDescriptor _colorPickerPassDescriptor;
    std::unique_ptr<RenderPass> _colorPickerRenderPass{nullptr};
    ColorPickerSubpass* _colorPickerSubpass{nullptr};
    
    std::array<uint8_t, 4> _readColorFromRenderTarget();
};

}

#endif /* editor_application_hpp */
