//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cloth_controller_hpp
#define cloth_controller_hpp

#include "job_manager.h"
#include "cloth_renderer.h"
#include "singleton.h"
#include "input_events.h"
#include <NvCloth/Solver.h>
#include <NvCloth/Fabric.h>
#include <NvCloth/Cloth.h>
#include <map>

namespace vox {
namespace cloth {
class ClothController : public Singleton<ClothController> {
public:
    static ClothController &getSingleton(void);
    
    static ClothController *getSingletonPtr(void);
    
    struct ClothActor {
        cloth::ClothRenderer *clothRenderer{nullptr};
        nv::cloth::Cloth *cloth{nullptr};
    };
    
    ClothController();
    
    ~ClothController();
    
    nv::cloth::Factory *factory();
    
    void update(float deltaTime);
    
    void handlePickingEvent(Camera* mainCamera, const InputEvent &inputEvent);
    
public:
    //Helper functions to enable automatic deinitialize
    //Tracking an object will delete it when autoDeinitialize is called
    //Untracking can be used if you delete it sooner than autoDeinitialize
    void trackClothActor(ClothActor *clothActor);
    
    void untrackClothActor(ClothActor *clothActor);
    
    void trackSolver(nv::cloth::Solver *solver);
    
    void untrackSolver(nv::cloth::Solver *solver);
    
    void trackFabric(nv::cloth::Fabric *fabric);
    
    void untrackFabric(nv::cloth::Fabric *fabric);
    
    //Help to detach cloths from solver at AutoDeinit.
    void addClothToSolver(ClothActor *clothActor, nv::cloth::Solver *solver);
    
    void addClothsToSolver(nv::cloth::Range<ClothActor *> clothActors, nv::cloth::Solver *solver);
    
private:
    void startSimulationStep(float dt);
    
    void waitForSimulationStep();
    
    void updateSimulationGraphics();
    
    void updateParticleDragging(const Ray3F& ray);
    
private:
    friend class ClothUI;

    nv::cloth::Factory *_factory{nullptr};
    std::vector<ClothActor *> _clothList;
    std::vector<nv::cloth::Solver *> _solverList;
    std::map<nv::cloth::Solver *, cloth::MultithreadedSolverHelper> _solverHelpers;
    std::vector<nv::cloth::Fabric *> _fabricList;
    std::map<ClothActor *, nv::cloth::Solver *> _clothSolverMap;
    
    cloth::JobManager _jobManager;
    
    //Particle dragging
    struct DraggingParticle {
        ClothActor* trackedCloth{nullptr};
        float dist = 0;
        float offset = 0;
        int particleIndex = 0;
    };
    DraggingParticle _draggingParticle;
};

}
}
#endif /* cloth_controller_hpp */
