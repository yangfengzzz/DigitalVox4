//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef shadow_subpass_hpp
#define shadow_subpass_hpp

#include "../subpass.h"
#include "graphics/mesh.h"
#include <vector>

namespace vox {
class ShadowSubpass: public Subpass {
public:
    ShadowSubpass(View* view,
                  Scene* scene,
                  Camera* camera);
    
    void setViewProjectionMatrix(const Matrix4x4F& vp);
    
    void prepare() override;
    
    void draw(MTL::RenderCommandEncoder& commandEncoder) override;
    
private:
    void drawMeshes(MTL::RenderCommandEncoder &renderEncoder);
    
    MTL::RenderPipelineState *_shadowGenPipelineState{nullptr};
    MTL::DepthStencilState _shadowDepthStencilState;
    Matrix4x4F _vp;
};

}

#endif /* shadow_subpass_hpp */
