//
//  shadow_subpass.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#include "shadow_subpass.h"

namespace vox {
ShadowSubpass::ShadowSubpass(MTL::RenderPassDescriptor* desc):
Subpass(desc) {
    
}

void ShadowSubpass::draw(MTL::CommandEncoder commandEncoder) {
    
}

}
