//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cloth_controller.h"
#include "camera.h"
#include "entity.h"
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

void ClothController::handlePickingEvent(Camera *mainCamera, const InputEvent &inputEvent) {
    if (inputEvent.source() == EventSource::Mouse) {
        const auto &mouse_button = static_cast<const MouseButtonInputEvent &>(inputEvent);
        if (mouse_button.action() == MouseAction::Down) {
            _draggingParticle.dist = 9999999.0f;
            _draggingParticle.offset = 9999999.0f;
            _draggingParticle.trackedCloth = nullptr;
            
            Ray3F ray = mainCamera->screenPointToRay(Vector2F(mouse_button.pos_x(), mouse_button.pos_y()));
            for (auto it: _clothList) {
                nv::cloth::Cloth *cloth = it->cloth;
                Matrix4x4F modelMatrix = it->clothRenderer->entity()->transform->worldMatrix();
                nv::cloth::Range<physx::PxVec4> particles = cloth->getCurrentParticles();
                
                for (int i = 0; i < (int) particles.size(); i++) {
                    physx::PxVec4 p = particles[i];
                    Point3F point(p.x, p.y, p.z);
                    point = modelMatrix * point;
                    
                    float dist = ray.direction.dot(point - ray.origin);
                    float offset = point.distanceTo(ray.origin + ray.direction * dist);
                    
                    if (offset < 0.1f) {
                        if (_draggingParticle.dist + 0.5f * _draggingParticle.offset > dist + 0.5f * offset) {
                            _draggingParticle.trackedCloth = it;
                            //mDraggingParticle.mOffset = offset;
                            _draggingParticle.dist = dist;
                            _draggingParticle.particleIndex = i;
                        }
                    }
                }
            }
        }
        
        if (mouse_button.action() == MouseAction::Move) {
            Ray3F ray = mainCamera->screenPointToRay(Vector2F(mouse_button.pos_x(), mouse_button.pos_y()));
            updateParticleDragging(ray);
        }
        
        if (mouse_button.action() == MouseAction::Up) {
            _draggingParticle.trackedCloth = nullptr;
        }
    }
}

void ClothController::updateParticleDragging(const Ray3F &ray) {
    if (_draggingParticle.trackedCloth) {
        nv::cloth::Cloth *cloth = _draggingParticle.trackedCloth->cloth;
        Matrix4x4F modelMatrix = _draggingParticle.trackedCloth->clothRenderer->entity()->transform->worldMatrix();
        nv::cloth::Range<physx::PxVec4> particles = cloth->getCurrentParticles();
        nv::cloth::Range<physx::PxVec4> prevParticles = cloth->getPreviousParticles();
        
        physx::PxVec3 particleLocal = particles[_draggingParticle.particleIndex].getXYZ();
        Point3F particleWorld = modelMatrix * Point3F(particleLocal.x, particleLocal.y, particleLocal.z);
        
        float rayT = _draggingParticle.dist;
        Point3F mousePointPlane = ray.origin + ray.direction * rayT;
        Vector3F offset = mousePointPlane - particleWorld;
        if (offset.lengthSquared() > 2.5f * 2.5f)
            offset = offset.normalized() * 2.5f;
        
        offset = modelMatrix.inverse() * offset;
        
        for (int i = 0; i < (int) particles.size(); i++) {
            physx::PxVec4 pLocal = particles[i];
            Vector4F p = modelMatrix * Vector4F(pLocal.x, pLocal.y, pLocal.z, pLocal.w);
            float dist = Point3F(p.x, p.y, p.z).distanceTo(particleWorld);
            
            //Only move dynamic points
            if (p.w > 0.0f) {
                const float softSelectionRadius = 0.4f;
                const float maxWeight = 0.4f;
                float weight = std::max(0.f, std::min(1.f, 1.f - (dist / softSelectionRadius))) * maxWeight;
                if (weight <= 0.0f)
                    continue;
                Point3F point0(prevParticles[i].x, prevParticles[i].y, prevParticles[i].z);
                point0 = point0 - weight * offset;
                point0 = point0 * 0.99f + Vector3F(p.x, p.y, p.z) * 0.01f;
                //move previous particle in the opposite direction to avoid invalid configurations in the next solver iteration.
                prevParticles[i] = physx::PxVec4(point0.x, point0.y, point0.z, prevParticles[i].w);
            }
        }
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
