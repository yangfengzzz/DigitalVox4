//
//  deferred_subpass.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#ifndef deferred_subpass_hpp
#define deferred_subpass_hpp

#include "../subpass.h"
#include "graphics/mesh.h"
#include <vector>

namespace vox {
class DeferredSubpass: public Subpass {
public:
    DeferredSubpass(MTL::RenderPassDescriptor* desc,
                    Scene* scene,
                    MTL::Library& shaderLibrary,
                    MTL::Device& m_device,
                    MTL::VertexDescriptor& m_defaultVertexDescriptor,
                    MTL::RenderPassDescriptor* shadow_desc);
    
    void draw(MTL::RenderCommandEncoder& commandEncoder) override;
    
private:
    void drawMeshes(MTL::RenderCommandEncoder &renderEncoder);

    MTL::RenderPipelineState m_GBufferPipelineState;
    MTL::DepthStencilState m_GBufferDepthStencilState;
    
    MTL::RenderPassDescriptor* shadow_desc;
};

}
#endif /* deferred_subpass_hpp */
