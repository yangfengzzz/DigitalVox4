//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef subpass_hpp
#define subpass_hpp

#include "core/cpp_mtl.h"
#include "scene.h"

namespace vox {
class RenderPass;

class Subpass {
public:
    Subpass(View* view,
            Scene* scene,
            Camera* camera);
    
    Subpass(const Subpass &) = delete;
    
    Subpass(Subpass &&) = default;
    
    virtual ~Subpass() = default;
    
    Subpass &operator=(const Subpass &) = delete;
    
    Subpass &operator=(Subpass &&) = delete;
    
    /**
     * @brief Prepares the shaders and shader variants for a subpass
     */
    virtual void prepare() = 0;
    
    /**
     * @brief Draw virtual function
     * @param commandEncoder CommandEncoder to use to record draw commands
     */
    virtual void draw(MTL::RenderCommandEncoder& commandEncoder) = 0;
    
    void setRenderPass(RenderPass* pass);
    
protected:
    RenderPass* _pass{nullptr};
    
    View* _view{nullptr};
    Scene* _scene{nullptr};
    Camera* _camera{nullptr};
};

}

#endif /* subpass_hpp */
