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

void PointLight::updateShadowMatrix() {
    auto projMatrix = shadowProjectionMatrix();
    auto worldPos = entity()->transform->worldPosition();
    shadow.lightPos = simd_make_float3(worldPos.x, worldPos.y, worldPos.z);
    
    for (int i = 0; i < 6; i++) {
        entity()->transform->lookAt(worldPos + cubeMapDirection[i].first, cubeMapDirection[i].second);
        auto viewMatrix = entity()->transform->worldMatrix().inverse();
        auto vp = projMatrix * viewMatrix;
        shadow.vp[0].columns[0] = simd_make_float4(vp[0], vp[1], vp[2], vp[3]);
        shadow.vp[0].columns[1] = simd_make_float4(vp[4], vp[5], vp[6], vp[7]);
        shadow.vp[0].columns[2] = simd_make_float4(vp[8], vp[9], vp[10], vp[11]);
        shadow.vp[0].columns[3] = simd_make_float4(vp[12], vp[13], vp[14], vp[15]);
    }
}

}
