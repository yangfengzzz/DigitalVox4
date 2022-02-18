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
};

}
}

#endif /* cloth_ui_hpp */
