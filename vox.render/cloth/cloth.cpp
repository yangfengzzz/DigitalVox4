//
//  cloth.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/2/17.
//

#include "cloth.h"
#include <NvCloth/Factory.h>

namespace vox {
void Cloth::init() {
    nv::cloth::Factory* factory = NvClothCreateFactoryCPU();
    if(factory == nullptr) {
        //error
    }
    
}

}
