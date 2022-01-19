//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef deferred_subpass_hpp
#define deferred_subpass_hpp

#include "../subpass.h"
#include "graphics/mesh.h"
#include <vector>

namespace vox {
class DeferredSubpass: public Subpass {
public:
    DeferredSubpass(View* view,
                    Scene* scene,
                    Camera* camera);
    
    void prepare() override;
    
    void draw(MTL::RenderCommandEncoder& commandEncoder) override;
    
    void setRenderPass(RenderPass* pass) override;

private:
    void drawMeshes(MTL::RenderCommandEncoder &renderEncoder);
    
    MTL::RenderPipelineDescriptor _GBufferPipelineDescriptor;
    MTL::DepthStencilState _GBufferDepthStencilState;
    
    MTL::Texture &_shadowMap;
};

}
#endif /* deferred_subpass_hpp */
