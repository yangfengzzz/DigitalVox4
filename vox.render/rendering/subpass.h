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
class Subpass {
public:
    Subpass(MTL::RenderPassDescriptor* desc,
            MTL::Device* device,
            Scene* scene,
            Camera* camera);
    
    Subpass(const Subpass &) = delete;
    
    Subpass(Subpass &&) = default;
    
    virtual ~Subpass() = default;
    
    Subpass &operator=(const Subpass &) = delete;
    
    Subpass &operator=(Subpass &&) = delete;
    
    /**
     * @brief Draw virtual function
     * @param commandEncoder CommandEncoder to use to record draw commands
     */
    virtual void draw(MTL::RenderCommandEncoder& commandEncoder) = 0;
    
protected:
    MTL::Device* _device;
    Scene* _scene;
    Camera* _camera;
};

}

#endif /* subpass_hpp */
