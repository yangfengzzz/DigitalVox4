//
//  scene_animation_app.hpp
//  apps
//
//  Created by 杨丰 on 2022/1/22.
//

#ifndef scene_animation_app_hpp
#define scene_animation_app_hpp

#include "forward_application.h"

namespace vox {
class SceneAnimationApp : public ForwardApplication {
public:
    void loadScene(uint32_t width, uint32_t height) override;
};
}

#endif /* scene_animation_app_hpp */
