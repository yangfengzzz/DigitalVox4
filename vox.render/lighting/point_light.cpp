//
//  point_light.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/19.
//

#include "point_light.h"
#include "scene.h"
#include "matrix_utils.h"

namespace vox {
PointLight::PointLight(Entity *entity) :
Light(entity) {
}

void PointLight::_onEnable() {
    scene()->light_manager.attachPointLight(this);
}

void PointLight::_onDisable() {
    scene()->light_manager.detachPointLight(this);
}

void PointLight::_updateShaderData(PointLightData &shaderData) {
    shaderData.color = simd_make_float3(color.r * intensity, color.g * intensity, color.b * intensity);
    auto position = entity()->transform->worldPosition();
    shaderData.position = simd_make_float3(position.x, position.y, position.z);
    shaderData.distance = distance;
}

//MARK: - Shadow
Matrix4x4F PointLight::shadowProjectionMatrix() {
    return makepPerspective<float>(degreesToRadians(90), 1, 0.1, 100);
}

}
