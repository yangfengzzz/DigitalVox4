//
//  gltf_app.hpp
//  apps
//
//  Created by 杨丰 on 2022/1/22.
//

#ifndef gltf_app_hpp
#define gltf_app_hpp

#include "forward_application.h"

namespace vox {
class GLTFApp : public ForwardApplication {
public:
    void loadScene(uint32_t width, uint32_t height) override;
};
}

#endif /* gltf_app_hpp */
