//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cloth_controller.h"
#include <NvClothExt/ClothFabricCooker.h>

namespace vox {
template<> cloth::ClothController *Singleton<cloth::ClothController>::msSingleton = 0;
//-----------------------------------------------------------------------
cloth::ClothController *cloth::ClothController::getSingletonPtr(void) {
    return msSingleton;
}

cloth::ClothController &cloth::ClothController::getSingleton(void) {
    assert(msSingleton);
    return (*msSingleton);
}

namespace cloth {
ClothController::ClothController() {
    NvClothEnvironment::AllocateEnv();
    _factory = NvClothCreateFactoryCPU();
    if (!_factory) {
        // bug
    }
}

ClothController::~ClothController() {
    //Remove all cloths from solvers
    for (auto it: _clothSolverMap) {
        it.second->removeCloth(it.first->cloth);
    }
    _clothSolverMap.clear();
    
    //Destroy all solvers
    for (auto it: _solverList) {
        delete it;
    }
    _solverList.clear();
    _solverHelpers.clear();
    
    //Destroy all cloths
    for (auto it: _clothList) {
        delete it->cloth;
    }
    _clothList.clear();
    
    //Destroy all fabrics
    for (auto it: _fabricList) {
        it->decRefCount();
    }
    _fabricList.clear();
}

nv::cloth::Factory *ClothController::factory() {
    return _factory;
}

void ClothController::update(float deltaTime) {
    startSimulationStep(deltaTime);
    waitForSimulationStep();
    updateSimulationGraphics();
}

void ClothController::startSimulationStep(float dt) {
    for (auto it = _solverHelpers.begin(); it != _solverHelpers.end(); ++it) {
        it->second.StartSimulation(dt);
    }
}

void ClothController::waitForSimulationStep() {
    for (auto it = _solverHelpers.begin(); it != _solverHelpers.end(); ++it) {
        it->second.WaitForSimulation();
    }
}

void ClothController::updateSimulationGraphics() {
    for (auto actor: _clothList) {
        nv::cloth::MappedRange<physx::PxVec4> particles = actor->cloth->getCurrentParticles();
        std::vector<physx::PxVec3> particles3(particles.size());
        for (uint32_t i = 0; i < particles.size(); ++i)
            particles3[i] = particles[i].getXYZ();
        
        actor->clothRenderer->update(particles3.data(), particles.size());
    }
}

namespace {
template<typename T>
void trackT(std::vector<T> &list, T object) {
    list.push_back(object);
}

template<typename T>
void untrackT(std::vector<T> &list, T object) {
    for (auto it = list.begin(); it != list.end(); ++it) {
        if (*it == object) {
            list.erase(it);
            break;
        }
    }
}
}

void ClothController::trackClothActor(ClothActor *clothActor) {
    trackT(_clothList, clothActor);
}

void ClothController::untrackClothActor(ClothActor *clothActor) {
    untrackT(_clothList, clothActor);
}

void ClothController::trackSolver(nv::cloth::Solver *solver) {
    trackT(_solverList, solver);
    _solverHelpers[solver].Initialize(solver, &_jobManager);
}

void ClothController::untrackSolver(nv::cloth::Solver *solver) {
    untrackT(_solverList, solver);
    _solverHelpers.erase(solver);
}

void ClothController::trackFabric(nv::cloth::Fabric *fabric) {
    trackT(_fabricList, fabric);
}

void ClothController::untrackFabric(nv::cloth::Fabric *fabric) {
    untrackT(_fabricList, fabric);
}

void ClothController::addClothToSolver(ClothActor *clothActor, nv::cloth::Solver *solver) {
    solver->addCloth(clothActor->cloth);
    assert(_clothSolverMap.find(clothActor) == _clothSolverMap.end());
    _clothSolverMap[clothActor] = solver;
}

void ClothController::addClothsToSolver(nv::cloth::Range<ClothActor *> clothActors, nv::cloth::Solver *solver) {
    // A temporary vector of Cloth*, to construct a Range from and pass to solver
    std::vector<nv::cloth::Cloth *> cloths;
    
    for (uint32_t i = 0; i < clothActors.size(); ++i) {
        auto *clothActor = clothActors[i];
        assert(_clothSolverMap.find(clothActor) == _clothSolverMap.end());
        _clothSolverMap[clothActor] = solver;
        
        cloths.push_back(clothActor->cloth);
    }
    
    auto clothsRange = nv::cloth::Range<nv::cloth::Cloth *>(&cloths.front(), &cloths.back() + 1);
    solver->addCloths(clothsRange);
}


}
}
