//
//  spherical.cpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/30.
//

#include "spherical.h"
#include "constants.h"

namespace vox {
namespace control {
Spherical::Spherical(float radius, float phi, float theta) :
radius(radius),
phi(phi),
theta(theta) {
}

void Spherical::set(float radius, float phi, float theta) {
    this->radius = radius;
    this->phi = phi;
    this->theta = theta;
}

void Spherical::makeSafe() {
    this->phi = clamp<float>(phi, kEpsilonF, M_PI - kEpsilonF);
}

void Spherical::setFromVec3(const Vector3F &v3) {
    radius = v3.length();
    if (radius == 0) {
        theta = 0;
        phi = 0;
    } else {
        theta = std::atan2(v3.x, v3.z);
        phi = std::acos(clamp<float>(v3.y / radius, -1, 1));
    }
}

void Spherical::setToVec3(Vector3F &v3) {
    const auto sinPhiRadius = std::sin(phi) * radius;
    
    v3.x = sinPhiRadius * std::sin(theta);
    v3.y = std::cos(phi) * radius;
    v3.z = sinPhiRadius * std::cos(theta);
}

}
}
