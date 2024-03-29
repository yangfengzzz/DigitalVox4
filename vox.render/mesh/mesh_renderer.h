//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef mesh_renderer_hpp
#define mesh_renderer_hpp

#include "renderer.h"

namespace vox {
class MeshRenderer : public Renderer {
public:
    explicit MeshRenderer(Entity *entity);
    
    /**
     * Mesh assigned to the renderer.
     */
    void setMesh(const MeshPtr &mesh);
    
    MeshPtr mesh();
    
private:
    void _render(std::vector<RenderElement> &opaqueQueue,
                 std::vector<RenderElement> &alphaTestQueue,
                 std::vector<RenderElement> &transparentQueue) override;
    
    void _updateBounds(BoundingBox3F &worldBounds) override;
    
private:
    MeshPtr _mesh;
    std::unique_ptr<UpdateFlag> _meshUpdateFlag;
};

}

#endif /* mesh_renderer_hpp */
