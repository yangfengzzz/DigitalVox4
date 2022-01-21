//
//  gui.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/21.
//

#ifndef gui_hpp
#define gui_hpp

#include "gui/imgui.h"
#include "core/cpp_mtl.h"

namespace vox {
class GUI {
public:
    GUI(MTL::Device* device);
    
    ~GUI();
    
    void newFrame(MTL::RenderPassDescriptor* renderPassDescriptor);
    
    void draw(ImDrawData* drawData,
              MTL::CommandBuffer& commandBuffer,
              MTL::RenderCommandEncoder& commandEncoder);
};

}

#endif /* gui_hpp */
