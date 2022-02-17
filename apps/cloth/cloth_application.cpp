//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cloth_application.h"
#include <NvClothExt/ClothFabricCooker.h>

namespace vox {
namespace cloth {
using namespace physx;
bool ClothApplication::prepare(Engine &engine) {
    ForwardApplication::prepare(engine);
    
    _factory = NvClothCreateFactoryCPU();
    if (!_factory) {
        // bug
    }
    
    return true;
}

void ClothApplication::update(float delta_time) {
    
}


}
}
