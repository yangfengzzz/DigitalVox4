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

GUI::~GUI() {
    ImGui_ImplMetal_Shutdown();
}

void GUI::newFrame(MTL::RenderPassDescriptor* renderPassDescriptor) {
    ImGui_ImplMetal_NewFrame(renderPassDescriptor->objCObj());
}

void GUI::draw(ImDrawData* drawData,
               MTL::CommandBuffer& commandBuffer,
               MTL::RenderCommandEncoder& commandEncoder) {
    ImGui_ImplMetal_RenderDrawData(drawData, commandBuffer.objCObj(), commandEncoder.objCObj());
}

}
