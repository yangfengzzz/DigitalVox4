//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef render_pass_hpp
#define render_pass_hpp

#include "subpass.h"
#include <vector>

namespace vox {
class RenderPass {
public:
    RenderPass(MTL::Device* device, MTL::RenderPassDescriptor* desc);
    
    RenderPass(const RenderPass &) = delete;
    
    RenderPass(RenderPass &&) = default;
    
    virtual ~RenderPass() = default;
    
    RenderPass &operator=(const RenderPass &) = delete;
    
    RenderPass &operator=(RenderPass &&) = default;
    
    void draw(MTL::CommandBuffer& commandBuffer,
              std::optional<std::string> label = std::nullopt);
    
public:
    /**
     * @brief Appends a subpass to the pipeline
     * @param subpass Subpass to append
     */
    void addSubpass(std::unique_ptr<Subpass> &&subpass);
    
    std::vector<std::unique_ptr<Subpass>> &subpasses();
    
    /**
     * @return Subpass currently being recorded, or the first one
     *         if drawing has not started
     */
    std::unique_ptr<Subpass> &activeSubpass();

    MTL::Library& library();
    
    const MTL::RenderPassDescriptor* renderPassDescriptor();

private:
    void makeShaderLibrary();
    
private:
    MTL::RenderPassDescriptor* _desc{nullptr};
    std::vector<std::unique_ptr<Subpass>> _subpasses;
    size_t _activeSubpassIndex{0};
    
    MTL::Library _library;
    MTL::Device* _device{nullptr};
};

}

#endif /* render_pass_hpp */
