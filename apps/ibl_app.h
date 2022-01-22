//
//  ibl_app.hpp
//  apps
//
//  Created by 杨丰 on 2022/1/22.
//

#ifndef ibl_app_hpp
#define ibl_app_hpp

#include "forward_application.h"
#include "controls/orbit_control.h"

namespace vox {
class IBLApp : public ForwardApplication {
public:
    bool prepare(Engine &engine) override;
    
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
    
    std::string _path;
    std::array<std::string, 6> _images;
};
}

#endif /* ibl_app_hpp */
