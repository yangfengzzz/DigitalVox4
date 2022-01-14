//
//  subpass.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#ifndef subpass_hpp
#define subpass_hpp

#include "core/CPPMetal.hpp"
#include "scene.h"

namespace vox {
class Subpass {
public:
    Subpass(MTL::RenderPassDescriptor* desc,
            sg::Scene* scene);
    
    Subpass(const Subpass &) = delete;
    
    Subpass(Subpass &&) = default;
    
    virtual ~Subpass() = default;
    
    Subpass &operator=(const Subpass &) = delete;
    
    Subpass &operator=(Subpass &&) = delete;
    
    /**
     * @brief Draw virtual function
     * @param commandEncoder CommandEncoder to use to record draw commands
     */
    virtual void draw(MTL::RenderCommandEncoder commandEncoder) = 0;
    
protected:
    sg::Scene* scene;
};

}

#endif /* subpass_hpp */
