//
//  omni_shadowMap_app.hpp
//  apps
//
//  Created by 杨丰 on 2022/1/22.
//

#ifndef omni_shadowMap_app_hpp
#define omni_shadowMap_app_hpp

#include "forward_application.h"
#include "controls/orbit_control.h"

namespace vox {
class OminiShadowMapApp : public ForwardApplication {
public:
    void loadScene(uint32_t width, uint32_t height) override;
};
}

#endif /* omni_shadowMap_app_hpp */
