//
//  cloth.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/2/17.
//

#include "cloth.h"
#include <NvCloth/Factory.h>
#include <NvCloth/Solver.h>
#include <NvClothExt/ClothFabricCooker.h>

namespace vox {
void Cloth::init() {
    nv::cloth::Factory* factory = NvClothCreateFactoryCPU();
    if(factory == nullptr) {
        //error
    }
    
    nv::cloth::ClothMeshDesc meshDesc;
    //Fill meshDesc with data
    meshDesc.setToDefault();
    physx::PxVec3 gravity(0.0f, -9.8f, 0.0f);
    nv::cloth::Vector<int32_t>::Type phaseTypeInfo;
    nv::cloth::Fabric* fabric = NvClothCookFabricFromMesh(factory, meshDesc, gravity, &phaseTypeInfo);
    
    nv::cloth::Cloth* cloth = factory->createCloth(nv::cloth::Range<physx::PxVec4>(NULL, NULL), *fabric);
    
    nv::cloth::Solver* solver = factory->createSolver();
    solver->addCloth(cloth);
    solver->endSimulation();
}

}
