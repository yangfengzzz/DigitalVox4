//
//  cascade_shadowmap_app.hpp
//  apps
//
//  Created by 杨丰 on 2022/1/23.
//

#ifndef cascade_shadowmap_app_hpp
#define cascade_shadowmap_app_hpp

#include "forward_application.h"

namespace vox {
class CascadeShadowMapApp : public ForwardApplication {
public:
    void loadScene(uint32_t width, uint32_t height) override;
};
}

#endif /* cascade_shadowmap_app_hpp */
