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
    
    Subpass(const Subpass &) = delete;
    
    Subpass(Subpass &&) = default;
    
    virtual ~Subpass() = default;
    
    Subpass &operator=(const Subpass &) = delete;
    
    Subpass &operator=(Subpass &&) = delete;
    
    /**
     * @brief Draw virtual function
     * @param commandEncoder CommandEncoder to use to record draw commands
     */
    virtual void draw(MTL::CommandEncoder commandEncoder) = 0;
};

}

#endif /* subpass_hpp */
