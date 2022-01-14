//
//  subpass.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#ifndef subpass_hpp
#define subpass_hpp

#include "core/CPPMetal.hpp"

namespace vox {
class Subpass {
public:
    Subpass(MTL::RenderPassDescriptor* desc);
    
    void draw(MTL::CommandEncoder commandEncoder);
};

}

#endif /* subpass_hpp */
