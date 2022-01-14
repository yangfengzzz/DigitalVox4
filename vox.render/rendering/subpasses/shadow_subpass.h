//
//  shadow_subpass.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#ifndef shadow_subpass_hpp
#define shadow_subpass_hpp

#include "../subpass.h"

namespace vox {
class ShadowSubpass: public Subpass {
public:
    ShadowSubpass(MTL::RenderPassDescriptor* desc);

    void draw(MTL::CommandEncoder commandEncoder) override;
};

}

#endif /* shadow_subpass_hpp */
