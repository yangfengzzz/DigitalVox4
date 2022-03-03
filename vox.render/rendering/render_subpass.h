//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef render_subpass_hpp
#define render_subpass_hpp

#include "subpass.h"
#include "shader/shader_uniform.h"

namespace vox {
class RenderPass;

class RenderSubpass: public Subpass {
public:
    Subpass::Type type() override final;
    
    RenderSubpass(RenderContext *context,
                  Scene *scene,
                  Camera *camera);
    
    RenderSubpass(const RenderSubpass &) = delete;
    
    RenderSubpass(RenderSubpass &&) = default;
    
    virtual ~RenderSubpass() = default;
    
    RenderSubpass &operator=(const RenderSubpass &) = delete;
    
    RenderSubpass &operator=(RenderSubpass &&) = delete;
    
    /**
     * @brief Draw virtual function
     * @param commandEncoder CommandEncoder to use to record draw commands
     */
    virtual void draw(MTL::RenderCommandEncoder &commandEncoder) = 0;
        
public:
    /**
     * Upload constant shader data in shader uniform block.
     * @param uniformBlock - shader Uniform block
     * @param shaderData - shader data
     */
    void uploadUniforms(MTL::RenderCommandEncoder &commandEncoder,
                        const std::vector<ShaderUniform> &uniformBlock,
                        const ShaderData &shaderData);
    
protected:
    static bool _compareFromNearToFar(const RenderElement &a, const RenderElement &b);
    
    static bool _compareFromFarToNear(const RenderElement &a, const RenderElement &b);
    
private:
    void process(const ShaderUniform &uniform, const std::any &a, MTL::RenderCommandEncoder &encoder);
};

}
#endif /* render_subpass_hpp */
