//
//  renderer.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/12.
//

#ifndef renderer_hpp
#define renderer_hpp

#include "component.h"
#include "shader/shader_data.h"
#include "rendering/render_element.h"
#include "bounding_box3.h"
#include "ImathMatrix.h"
#include "updateFlag.h"

namespace vox {
/**
 * Renderable component.
 */
class Renderer : public Component {
public:
    /** ShaderData related to renderer. */
    ShaderData shaderData = ShaderData();
    // @ignoreClone
    /** Whether it is clipped by the frustum, needs to be turned on camera.enableFrustumCulling. */
    bool isCulled = false;
    
    /** Set whether the renderer to receive shadows. */
    bool receiveShadow = false;
    /** Set whether the renderer to cast shadows. */
    bool castShadow = false;
    
    /**
     * Material count.
     */
    size_t materialCount();
    
    /**
     * The bounding volume of the renderer.
     */
    Imath::BoundingBox3f bounds();
    
    explicit Renderer(Entity *entity);
    
public:
    /**
     * Get the first instance material by index.
     * @remarks Calling this function for the first time after the material is set will create an instance material to ensure that it is unique to the renderer.
     * @param index - Material index
     * @returns Instance material
     */
    MaterialPtr getInstanceMaterial(size_t index = 0);
    
    /**
     * Get the first material by index.
     * @param index - Material index
     * @returns Material
     */
    MaterialPtr getMaterial(size_t index = 0);
    
    /**
     * Set the first material.
     * @param material - The first material
     */
    void setMaterial(MaterialPtr material);
    
    /**
     * Set material by index.
     * @param index - Material index
     * @param material - The material
     */
    void setMaterial(size_t index, MaterialPtr material);
    
    /**
     * Get all instance materials.
     * @remarks Calling this function for the first time after the material is set will create an instance material to ensure that it is unique to the renderer.
     * @returns All instance materials
     */
    std::vector<MaterialPtr> getInstanceMaterials();
    
    /**
     * Get all materials.
     * @returns All materials
     */
    std::vector<MaterialPtr> getMaterials();
    
    /**
     * Set all materials.
     * @param materials - All materials
     */
    void setMaterials(const std::vector<MaterialPtr> &materials);
    
//    void pushPrimitive(const RenderElement &element,
//                       std::vector<RenderElement> &opaqueQueue,
//                       std::vector<RenderElement> &alphaTestQueue,
//                       std::vector<RenderElement> &transparentQueue);
    
protected:
    void _onEnable() override;
    
    void _onDisable() override;
    
    void _onDestroy() override;
    
    virtual void _render(std::vector<RenderElement> &opaqueQueue,
                         std::vector<RenderElement> &alphaTestQueue,
                         std::vector<RenderElement> &transparentQueue) = 0;
    
    virtual void _updateBounds(Imath::BoundingBox3f &worldBounds) {
    }
    
    virtual void update(float deltaTime) {
    }
    
protected:
//    void _updateShaderData(RenderContext &context);
    
    MaterialPtr _createInstanceMaterial(const MaterialPtr &material, size_t index);
    
    float _distanceForSort = 0;
    ssize_t _onUpdateIndex = -1;
    ssize_t _rendererIndex = -1;
    ShaderMacroCollection _globalShaderMacro = ShaderMacroCollection();
    int _renderSortId = 0;
    
    // @ignoreClone
    bool _overrideUpdate = false;
    // @shallowClone
    std::vector<std::shared_ptr<Material>> _materials;
    
private:
    friend class ComponentsManager;

    ShaderProperty _localMatrixProperty;
    ShaderProperty _worldMatrixProperty;
    ShaderProperty _mvMatrixProperty;
    ShaderProperty _mvpMatrixProperty;
    ShaderProperty _mvInvMatrixProperty;
    ShaderProperty _normalMatrixProperty;
    
    std::unique_ptr<UpdateFlag> _transformChangeFlag;
    // @deepClone
    Imath::BoundingBox3f _bounds = Imath::BoundingBox3f();
    // @ignoreClone
    Imath::M44f _mvMatrix = Imath::M44f();
    // @ignoreClone
    Imath::M44f _mvpMatrix = Imath::M44f();
    // @ignoreClone
    Imath::M44f _mvInvMatrix = Imath::M44f();
    // @ignoreClone
    Imath::M44f _normalMatrix = Imath::M44f();
    // @ignoreClone
    std::vector<bool> _materialsInstanced;
};


}

#endif /* renderer_hpp */
