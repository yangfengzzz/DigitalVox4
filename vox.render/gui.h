//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef gui_hpp
#define gui_hpp

#include "gui/imgui.h"
#include <Metal/Metal.hpp>

namespace vox {
class GUI {
public:
    GUI(const std::shared_ptr<MTL::Device>& device);
    
    ~GUI();
    
    void newFrame(const std::shared_ptr<MTL::RenderPassDescriptor>& renderPassDescriptor);
    
    void draw(ImDrawData* drawData,
              const std::shared_ptr<MTL::CommandBuffer>& commandBuffer,
              const std::shared_ptr<MTL::RenderCommandEncoder>& commandEncoder);
};

}

#endif /* gui_hpp */
