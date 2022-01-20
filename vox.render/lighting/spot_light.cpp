//
//  spot_light.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/19.
//

#include "spot_light.h"
#include "scene.h"
#include "matrix_utils.h"

namespace vox {
SpotLight::SpotLight(Entity *entity) :
Light(entity) {
}

void SpotLight::_onEnable() {
    scene()->light_manager.attachSpotLight(this);
}

void SpotLight::_onDisable() {
    scene()->light_manager.detachSpotLight(this);
}

void SpotLight::_updateShaderData(SpotLightData &shaderData) {
    shaderData.color = simd_make_float3(color.r * intensity, color.g * intensity, color.b * intensity);
    auto position = entity()->transform->worldPosition();
    shaderData.position = simd_make_float3(position.x, position.y, position.z);
    auto direction = entity()->transform->worldForward();
    shaderData.direction = simd_make_float3(direction.x, direction.y, direction.z);
    shaderData.distance = distance;
    shaderData.angleCos = std::cos(angle);
    shaderData.penumbraCos = std::cos(angle + penumbra);
}

// MARK: - Shadow
Matrix4x4F SpotLight::shadowProjectionMatrix() {
    const auto fov = std::min(M_PI / 2, angle * 2 * std::sqrt(2));
    return makepPerspective<float>(fov, 1, 0.1, distance + 5);
}

void SpotLight::updateShadowMatrix() {
    auto viewMatrix = entity()->transform->worldMatrix().inverse();
    auto projMatrix = shadowProjectionMatrix();
    auto vp = projMatrix * viewMatrix;
    shadow.vp[0].columns[0] = simd_make_float4(vp[0], vp[1], vp[2], vp[3]);
    shadow.vp[0].columns[1] = simd_make_float4(vp[4], vp[5], vp[6], vp[7]);
    shadow.vp[0].columns[2] = simd_make_float4(vp[8], vp[9], vp[10], vp[11]);
    shadow.vp[0].columns[3] = simd_make_float4(vp[12], vp[13], vp[14], vp[15]);
    shadow.cascadeSplits[0] = 1;
    shadow.cascadeSplits[1] = -1; // mark cascade with negative sign
}

}