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

}
