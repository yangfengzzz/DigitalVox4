//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef shadow_subpass_hpp
#define shadow_subpass_hpp

#include "rendering/subpass.h"
#include "graphics/mesh.h"
#include <vector>

namespace vox {
class ShadowSubpass : public Subpass {
public:
    ShadowSubpass(RenderContext *context,
                  Scene *scene,
                  Camera *camera);
    
    void setViewProjectionMatrix(const Matrix4x4F &vp);
    
    void setViewport(const std::optional<MTL::Viewport> &viewport);
    
    void prepare() override;
    
    void draw(MTL::RenderCommandEncoder &commandEncoder) override;
    
private:
    void drawMeshes(MTL::RenderCommandEncoder &renderEncoder);
    
    std::shared_ptr<MTL::RenderPipelineDescriptor> _shadowGenDescriptor;
    std::shared_ptr<MTL::DepthStencilState> _shadowDepthStencilState;
    Matrix4x4F _vp;
    std::optional<MTL::Viewport> _viewport = std::nullopt;
};

}

#endif /* shadow_subpass_hpp */
