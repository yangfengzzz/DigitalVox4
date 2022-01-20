//
//  color_picker_subpass.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/20.
//

#ifndef color_picker_subpass_hpp
#define color_picker_subpass_hpp

#include "../subpass.h"

namespace vox {
class ColorPickerSubpass: public Subpass {
public:
    ColorPickerSubpass(View* view,
                       Scene* scene,
                       Camera* camera);
    
    void prepare() override;
    
    void draw(MTL::RenderCommandEncoder& commandEncoder) override;
    
private:
    static bool _compareFromNearToFar(const RenderElement &a, const RenderElement &b);
    static bool _compareFromFarToNear(const RenderElement &a, const RenderElement &b);
    
    void _drawMeshes(MTL::RenderCommandEncoder &renderEncoder);
    
    void _drawElement(MTL::RenderCommandEncoder &renderEncoder,
                      const std::vector<RenderElement> &items,
                      const ShaderMacroCollection& compileMacros);
    
    MTL::RenderPipelineDescriptor _forwardPipelineDescriptor;
    MTL::DepthStencilState _forwardDepthStencilState;
};

}

#endif /* color_picker_subpass_hpp */
