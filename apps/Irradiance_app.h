//
//  Irradiance_app.hpp
//  apps
//
//  Created by 杨丰 on 2022/1/22.
//

#ifndef Irradiance_app_hpp
#define Irradiance_app_hpp

#include "forward_application.h"
#include "controls/orbit_control.h"

namespace vox {
class IrradianceApp : public ForwardApplication {
public:
    void loadScene(uint32_t width, uint32_t height) override;
};
}

#endif /* Irradiance_app_hpp */
