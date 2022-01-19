//
//  point_light.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/19.
//

#ifndef point_light_hpp
#define point_light_hpp

#include "light.h"
#include "color.h"
#include "shader_types.h"

namespace vox {
/**
 * Point light.
 */
class PointLight : public Light {
public:
    /** Light color. */
    Color color = Color(1, 1, 1, 1);
    /** Light intensity. */
    float intensity = 1.0;
    /** Defines a distance cutoff at which the light's intensity must be considered zero. */
    float distance = 100;
    
    PointLight(Entity *entity);
    
public:
    Matrix4x4F shadowProjectionMatrix() override;
    
    void updateShadowMatrix();
    
    CubeShadowData shadow;
    
private:
    /**
     * Mount to the current Scene.
     */
    void _onEnable() override;
    
    /**
     * Unmount from the current Scene.
     */
    void _onDisable() override;
    
    void _updateShaderData(PointLightData &shaderData);
    
private:
    friend class LightManager;
    
    const std::array<std::pair<Vector3F, Vector3F>, 6> cubeMapDirection = {
        std::make_pair(Vector3F(10, 0, 0), Vector3F(0, 1, 0)),
        std::make_pair(Vector3F(-10, 0, 0), Vector3F(0, 1, 0)),
        std::make_pair(Vector3F(0, 10, 0), Vector3F(1, 0, 0)),
        std::make_pair(Vector3F(0, -10, 0), Vector3F(1, 0, 0)),
        std::make_pair(Vector3F(0, 0, 10), Vector3F(0, 1, 0)),
        std::make_pair(Vector3F(0, 0, -10), Vector3F(0, 1, 0)),
    };
};

}

#endif /* point_light_hpp */
