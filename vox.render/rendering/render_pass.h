//
//  render_pass.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/14.
//

#ifndef render_pass_hpp
#define render_pass_hpp

#include "subpass.h"
#include <vector>

namespace vox {
class RenderPass {
public:
    RenderPass(MTL::RenderPassDescriptor* desc);
    
    RenderPass(const RenderPass &) = delete;
    
    RenderPass(RenderPass &&) = default;
    
    virtual ~RenderPass() = default;
    
    RenderPass &operator=(const RenderPass &) = delete;
    
    RenderPass &operator=(RenderPass &&) = default;
    
    void draw(MTL::CommandBuffer& commandBuffer);
    
public:
    /**
     * @brief Appends a subpass to the pipeline
     * @param subpass Subpass to append
     */
    void addSubpass(std::unique_ptr<Subpass> &&subpass);
    
    std::vector<std::unique_ptr<Subpass>> &getSubpasses();
    
    /**
     * @return Subpass currently being recorded, or the first one
     *         if drawing has not started
     */
    std::unique_ptr<Subpass> &getActiveSubpass();
    
private:
    MTL::RenderPassDescriptor* desc;
    std::vector<std::unique_ptr<Subpass>> subpasses;
    size_t active_subpass_index{0};
};

}

#endif /* render_pass_hpp */
