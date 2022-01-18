//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef blinn_phong_material_hpp
#define blinn_phong_material_hpp

#include "base_material.h"
#include "color.h"
#include "vector4.h"

namespace vox {
/**
 * Blinn-phong Material.
 */
class BlinnPhongMaterial : public BaseMaterial {
public:
    /**
     * Base color.
     */
    Color baseColor();
    
    void setBaseColor(const Color &newValue);
    
    /**
     * Base texture.
     */
    std::shared_ptr<MTL::Texture> baseTexture();
    
    void setBaseTexture(std::shared_ptr<MTL::Texture> newValue);
    
    /**
     * Specular color.
     */
    Color specularColor();
    
    void setSpecularColor(const Color &newValue);
    
    /**
     * Specular texture.
     */
    std::shared_ptr<MTL::Texture> specularTexture();
    
    void setSpecularTexture(std::shared_ptr<MTL::Texture> newValue);
    
    /**
     * Emissive color.
     */
    Color emissiveColor();
    
    void setEmissiveColor(const Color &newValue);
    
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
    Vector4F tilingOffset();
    
    void setTilingOffset(const Vector4F &newValue);
    
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
