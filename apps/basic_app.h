//
//  basic_apps.hpp
//  apps
//
//  Created by 杨丰 on 2022/1/19.
//

#ifndef basic_apps_h
#define basic_apps_h

#include "forward_application.h"
#include "controls/orbit_control.h"

namespace vox {
class BasicApp : public ForwardApplication {
public:
    void loadScene(uint32_t width, uint32_t height) override;
    
    void inputEvent(const InputEvent &inputEvent) override;

private:
    control::OrbitControl* _controller{nullptr};
};
}

#endif /* basic_apps_hpp */
