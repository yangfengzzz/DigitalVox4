//
//  blinn_phong_material.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/29.
//

#ifndef blinn_phong_material_hpp
#define blinn_phong_material_hpp

#include "base_material.h"
#include "ImathColor.h"

namespace vox {
/**
 * Blinn-phong Material.
 */
class BlinnPhongMaterial : public BaseMaterial {
public:
    /**
     * Base color.
     */
    Imath::Color4f baseColor();
    
    void setBaseColor(const Imath::Color4f &newValue);
    
    /**
     * Base texture.
     */
    std::shared_ptr<MTL::Texture> baseTexture();
    
    void setBaseTexture(std::shared_ptr<MTL::Texture> newValue);
    
    /**
     * Specular color.
     */
    Imath::Color4f specularColor();
    
    void setSpecularColor(const Imath::Color4f &newValue);
    
    /**
     * Specular texture.
     */
    std::shared_ptr<MTL::Texture> specularTexture();
    
    void setSpecularTexture(std::shared_ptr<MTL::Texture> newValue);
    
    /**
     * Emissive color.
     */
    Imath::Color4f emissiveColor();
    
    void setEmissiveColor(const Imath::Color4f &newValue);
    
    /**
     * Emissive texture.
     */
    std::shared_ptr<MTL::Texture> emissiveTexture();
    
    void setEmissiveTexture(std::shared_ptr<MTL::Texture> newValue);
    
    /**
     * Normal texture.
     */
    std::shared_ptr<MTL::Texture> normalTexture();
    
    void setNormalTexture(std::shared_ptr<MTL::Texture> newValue);
    
    /**
     * Normal texture intensity.
     */
    float normalIntensity();
    
    void setNormalIntensity(float newValue);
    
    /**
     * Set the specular reflection coefficient, the larger the value, the more convergent the specular reflection effect.
     */
    float shininess();
    
    void setShininess(float newValue);
    
    /**
     * Tiling and offset of main textures.
     */
    Imath::V4f tilingOffset();
    
    void setTilingOffset(const Imath::V4f &newValue);
    
    explicit BlinnPhongMaterial();
    
private:
    ShaderProperty _diffuseColorProp;
    ShaderProperty _specularColorProp;
    ShaderProperty _emissiveColorProp;
    ShaderProperty _tilingOffsetProp;
    ShaderProperty _shininessProp;
    ShaderProperty _normalIntensityProp;
    
    ShaderProperty _baseTextureProp;
    ShaderProperty _specularTextureProp;
    ShaderProperty _emissiveTextureProp;
    ShaderProperty _normalTextureProp;
};


}

#endif /* blinn_phong_material_hpp */
