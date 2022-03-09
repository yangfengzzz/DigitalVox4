//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cloth_editor_hpp
#define cloth_editor_hpp

#include "editor_application.h"
#include "gui_entry.h"
#include "cloth/cloth_controller.h"

namespace vox {
namespace cloth {
class ClothEditor : public EditorApplication {
public:
    ClothEditor();
    
    ~ClothEditor();
    
    void loadScene() override;
    
    void pickFunctor(Renderer *renderer, MeshPtr mesh) override;
    
    void inputEvent(const InputEvent &inputEvent) override;
    
    void update(float delta_time) override;
    
private:
    editor::GUIEntry* _entry{nullptr};
    ClothController* _clothController{nullptr};
    
    void _initializeCloth(EntityPtr entity, physx::PxVec3 offset);
    nv::cloth::Fabric *_fabric{nullptr};
    nv::cloth::Solver *_solver{nullptr};
    ClothController::ClothActor _clothActor;
};

} 
}

#endif /* cloth_editor_hpp */
