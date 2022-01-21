//
//  gui.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/21.
//

#include "gui.h"
#include "gui/imgui_impl_metal.h"

namespace vox {
GUI::GUI(MTL::Device* device) {
    ImGui_ImplMetal_Init(device->objCObj());
}

}
