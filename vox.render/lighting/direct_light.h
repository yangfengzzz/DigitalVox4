//
//  direct_light.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/19.
//

#ifndef direct_light_hpp
#define direct_light_hpp

#include "light.h"
#include "color.h"
#include "shader_common.h"

namespace vox {
/**
 * Directional light.
 */
class DirectLight : public Light {
public:
    /** Light color. */
    Color color = Color(1, 1, 1, 1);
    /** Light intensity. */
    float intensity = 1.0;
    
    DirectLight(Entity *entity);
    
public:
    Matrix4x4F shadowProjectionMatrix() override;
    
    Vector3F direction();
        
private:
    friend class LightManager;
    
    /**
     * Mount to the current Scene.
     */
    void _onEnable() override;
    
    /**
     * Unmount from the current Scene.
     */
    void _onDisable() override;
    
    void _updateShaderData(DirectLightData &shaderData);
};

}

#endif /* direct_light_hpp */
