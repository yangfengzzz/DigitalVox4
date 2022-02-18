//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cloth_ui_hpp
#define cloth_ui_hpp

#include "../editor_component.h"

namespace vox {
namespace cloth {
class ClothApplication;

class ClothUI : public editor::EditorComponent {
public:
    void onUpdate() override;
    
private:
    void _updateClothUI();
    
    void _updateSolverUI();

    void _updateDebugUI();

    void _drawDebugVisualization();
    
    void _debugRenderDistanceConstraints();
    void _debugRenderTethers();
    void _debugRenderConstraints();
    void _debugRenderConstraintStiffness();
    void _debugRenderConstraintError();
    void _debugRenderPositionDelta();
    void _debugRenderBoundingBox();
    
    enum {
        DEBUG_VIS_DISTANCE_CONSTRAINTS = 1,
        DEBUG_VIS_TETHERS = 2,
        DEBUG_VIS_CONSTRAINTS = 4,
        DEBUG_VIS_CONSTRAINTS_STIFFNESS = 8,
        DEBUG_VIS_NORMALS = 16,
        DEBUG_VIS_TANGENTS = 32,
        DEBUG_VIS_BITANGENTS = 64,
        DEBUG_VIS_CONSTRAINT_ERROR = 128,
        DEBUG_VIS_POSITION_DELTA = 256,
        DEBUG_VIS_ACCELERATION = 512,
        DEBUG_VIS_BOUNDING_BOX = 1024,
        DEBUG_VIS_LAST
    };
    static unsigned int _debugVisualizationFlags;
    static bool _debugVisualizationUpdateRequested;
    
    struct SceneDebugRenderParams {
        //Constraint render params
        int visiblePhaseRangeBegin;
        int visiblePhaseRangeEnd;
    };
    static SceneDebugRenderParams _sceneDebugRenderParams;
};

}
}

#endif /* cloth_ui_hpp */
