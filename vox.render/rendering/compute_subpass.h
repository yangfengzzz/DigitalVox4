//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef compute_subpass_hpp
#define compute_subpass_hpp

#include "subpass.h"
#include "shader/shader_uniform.h"

namespace vox {
class ComputeSubpass: public Subpass {
public:
    Subpass::Type type() override final;
    
    ComputeSubpass(RenderContext *context,
                   Scene *scene,
                   Camera *camera);
    
    ComputeSubpass(const ComputeSubpass &) = delete;
    
    ComputeSubpass(ComputeSubpass &&) = default;
    
    virtual ~ComputeSubpass() = default;
    
    ComputeSubpass &operator=(const ComputeSubpass &) = delete;
    
    ComputeSubpass &operator=(ComputeSubpass &&) = delete;
    
    /**
     * @brief Compute virtual function
     * @param commandEncoder CommandEncoder to use to record compute commands
     */
    virtual void compute(MTL::ComputeCommandEncoder &commandEncoder) = 0;
    
public:
    /**
     * Upload constant shader data in shader uniform block.
     * @param uniformBlock - shader Uniform block
     * @param shaderData - shader data
     */
    void uploadUniforms(MTL::ComputeCommandEncoder &commandEncoder,
                        const std::vector<ShaderUniform> &uniformBlock,
                        const ShaderData &shaderData);
    
private:
    void process(const ShaderUniform &uniform, const std::any &a, MTL::ComputeCommandEncoder &encoder);
};

}
#endif /* compute_subpass_hpp */
