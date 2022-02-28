//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "gui.h"
#include "gui/imgui_impl_metal.h"

namespace vox {
GUI::GUI(const std::shared_ptr<MTL::Device>& device) {
    ImGui_ImplMetal_Init(device.get());
}

GUI::~GUI() {
    ImGui_ImplMetal_Shutdown();
}

void GUI::newFrame(const std::shared_ptr<MTL::RenderPassDescriptor>& renderPassDescriptor) {
    ImGui_ImplMetal_NewFrame(renderPassDescriptor.get());
}

void GUI::draw(ImDrawData* drawData,
               const std::shared_ptr<MTL::CommandBuffer>& commandBuffer,
               const std::shared_ptr<MTL::RenderCommandEncoder>& commandEncoder) {
    ImGui_ImplMetal_RenderDrawData(drawData, commandBuffer.get(), commandEncoder.get());
}

}
