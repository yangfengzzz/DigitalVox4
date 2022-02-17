//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cloth_application_hpp
#define cloth_application_hpp

#include "forward_application.h"
#include <NvCloth/Solver.h>
#include <NvCloth/Fabric.h>
#include <NvCloth/Cloth.h>
#include <map>

#include "cloth/job_manager.h"
#include "cloth/cloth_renderer.h"

namespace vox {
namespace cloth {
class ClothApplication : public ForwardApplication {
public:
    struct ClothActor {
        ClothRenderer *clothRenderer{nullptr};
        nv::cloth::Cloth *cloth{nullptr};
    };
    
    bool prepare(Engine &engine) override;
    
    void update(float delta_time) override;
    
    void finish() override;
    
protected:
    nv::cloth::Factory *_factory{nullptr};
    
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
    
    void startSimulationStep(float dt);
    
    void waitForSimulationStep();
    
    void updateSimulationGraphics();
    
private:
    std::vector<ClothActor *> _clothList;
    std::vector<nv::cloth::Solver *> _solverList;
    std::map<nv::cloth::Solver *, MultithreadedSolverHelper> _solverHelpers;
    std::vector<nv::cloth::Fabric *> _fabricList;
    std::map<ClothActor *, nv::cloth::Solver *> _clothSolverMap;
    
    JobManager _jobManager;
};

}
}
#endif /* cloth_application_hpp */
