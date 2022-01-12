//
//  pbr_material.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/29.
//

#ifndef pbr_material_hpp
#define pbr_material_hpp

#include "pbr_base_material.h"

namespace vox {
/**
 * PBR (Metallic-Roughness Workflow) Material.
 */
class PBRMaterial : public PBRBaseMaterial {
public:
    /**
     * Metallic.
     */
    float metallic();
    
    void setMetallic(float newValue);
    
    /**
     * Roughness.
     */
    float roughness();
    
    void setRoughness(float newValue);
    
    /**
     * Roughness metallic texture.
     * @remarks G channel is roughness, B channel is metallic
     */
    std::shared_ptr<MTL::Texture> metallicRoughnessTexture();
    
    void setMetallicRoughnessTexture(std::shared_ptr<MTL::Texture> newValue);
    
    /**
     * Create a pbr metallic-roughness workflow material instance.
     */
    explicit PBRMaterial();
    
private:
    ShaderProperty _metallicProp;
    ShaderProperty _roughnessProp;
    ShaderProperty _metallicRoughnessTextureProp;
};

}

#endif /* pbr_material_hpp */
