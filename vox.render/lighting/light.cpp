//
//  light.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/19.
//

#include "light.h"
#include "entity.h"

namespace vox {
Light::Light(Entity *entity) :
Component(entity) {
}

Matrix4x4F Light::viewMatrix() {
    return entity()->transform->worldMatrix().inverse();
}

Matrix4x4F Light::inverseViewMatrix() {
    return entity()->transform->worldMatrix();
}

//MARK: - Shadow
bool Light::enableShadow() {
    return _enableShadow;
}

void Light::setEnableShadow(bool enabled) {
    _enableShadow = enabled;
}

float Light::shadowBias() {
    return _shadowBias;
}

void Light::setShadowBias(float value) {
    _shadowBias = value;
}

float Light::shadowIntensity() {
    return _shadowIntensity;
}

void Light::setShadowIntensity(float value) {
    _shadowIntensity = value;
}

float Light::shadowRadius() {
    return _shadowRadius;
}

void Light::setShadowRadius(float value) {
    _shadowRadius = value;
}

}
