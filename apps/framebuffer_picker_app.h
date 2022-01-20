//
//  framebuffer_picker_app.hpp
//  apps
//
//  Created by 杨丰 on 2022/1/20.
//

#ifndef framebuffer_picker_app_hpp
#define framebuffer_picker_app_hpp

#include "editor_application.h"
#include <random>

namespace vox {
class FramebufferPickerApp : public EditorApplication {
public:
    void loadScene(uint32_t width, uint32_t height) override;
    
    void pickFunctor(Renderer *renderer, MeshPtr mesh) override;
    
    void inputEvent(const InputEvent &inputEvent) override;
    
private:
    std::default_random_engine e;
    std::uniform_real_distribution<float> u;
};
 
}
#endif /* framebuffer_picker_app_hpp */
