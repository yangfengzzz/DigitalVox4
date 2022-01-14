//
//  deferred.cpp
//  apps
//
//  Created by 杨丰 on 2022/1/14.
//

#include "deferred.h"

namespace vox {
Deferred::~Deferred() {
    
}

bool Deferred::prepare(Engine &engine) {
    return MetalApplication::prepare(engine);
}

void Deferred::update(float delta_time) {
    MetalApplication::update(delta_time);
}

}
