//
//  mesh_renderer.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/15.
//

#ifndef mesh_renderer_hpp
#define mesh_renderer_hpp

#include "renderer.h"

namespace vox {
class MeshRenderer: public Renderer {
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
    
private:
    MeshPtr _mesh;
    std::unique_ptr<UpdateFlag> _meshUpdateFlag;
};

}

#endif /* mesh_renderer_hpp */
