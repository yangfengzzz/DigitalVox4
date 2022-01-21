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
    
public:
    /**
     * Convert id to RGB color value, 0 and 0xffffff are illegal values.
     */
    Vector3F id2Color(uint32_t id);
    
    /**
     * Convert RGB color to id.
     * @param color - Color
     */
    uint32_t color2Id(const std::array<uint8_t, 4> &color);
    
    /**
     * Get renderer element by color.
     */
    std::pair<Renderer *, MeshPtr> getObjectByColor(const std::array<uint8_t, 4> &color);
    
private:    
    void _drawMeshes(MTL::RenderCommandEncoder &renderEncoder);
    
    void _drawElement(MTL::RenderCommandEncoder &renderEncoder,
                      const std::vector<RenderElement> &items,
                      const ShaderMacroCollection& compileMacros);
    
    MTL::RenderPipelineDescriptor _forwardPipelineDescriptor;
    MTL::DepthStencilState _forwardDepthStencilState;
    
    uint32_t _currentId = 0;
    std::unordered_map<size_t, std::pair<Renderer *, MeshPtr>> _primitivesMap;
};

}

#endif /* color_picker_subpass_hpp */
