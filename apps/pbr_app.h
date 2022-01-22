//
//  pbr_app.hpp
//  apps
//
//  Created by 杨丰 on 2022/1/22.
//

#ifndef pbr_app_hpp
#define pbr_app_hpp

#include "forward_application.h"
#include "controls/orbit_control.h"

namespace vox {
class PBRApp : public ForwardApplication {
public:
    void loadScene(uint32_t width, uint32_t height) override;
    
private:
    struct Material {
        std::string name;
        Color baseColor;
        float roughness;
        float metallic;
        
        Material() {
        };
        
        Material(std::string n, Color c, float r, float m) : name(n) {
            roughness = r;
            metallic = m;
            baseColor = c;
        };
    };
    std::vector<Material> _materials;
};

}

#endif /* pbr_app_hpp */
