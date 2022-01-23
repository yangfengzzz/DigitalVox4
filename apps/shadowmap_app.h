//
//  shadowmap_app.hpp
//  apps
//
//  Created by 杨丰 on 2022/1/23.
//

#ifndef shadowmap_app_hpp
#define shadowmap_app_hpp

#include "forward_application.h"

namespace vox {
class ShadowMapApp : public ForwardApplication {
public:
    void loadScene(uint32_t width, uint32_t height) override;
};
}

#endif /* shadowmap_app_hpp */
