//
//  pbr_base_material.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/29.
//

#ifndef pbr_base_material_hpp
#define pbr_base_material_hpp

#include "base_material.h"
#include "color.h"
#include "vector4.h"

namespace vox {
/**
 * PBR (Physically-Based Rendering) Material.
 */
class PBRBaseMaterial : public BaseMaterial {
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
     * Normal texture.
     */
    std::shared_ptr<MTL::Texture> normalTexture();
    
    void setNormalTexture(std::shared_ptr<MTL::Texture> newValue);
    
    /**
     * Normal texture intensity.
     */
    float normalTextureIntensity();
    
    void setNormalTextureIntensity(float newValue);
    
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
     * Occlusion texture.
     */
    std::shared_ptr<MTL::Texture> occlusionTexture();
    
    void setOcclusionTexture(std::shared_ptr<MTL::Texture> newValue);
    
    /**
     * Occlusion texture intensity.
     */
    float occlusionTextureIntensity();
    
    void setOcclusionTextureIntensity(float newValue);
    
    /**
     * Tiling and offset of main textures.
     */
    Vector4F tilingOffset();
    
    void setTilingOffset(const Vector4F &newValue);
    
protected:
    /**
     * Create a pbr base material instance.
     */
    explicit PBRBaseMaterial();
    
private:
    ShaderProperty _tilingOffsetProp;
    ShaderProperty _normalTextureIntensityProp;
    ShaderProperty _occlusionTextureIntensityProp;
    
    ShaderProperty _baseColorProp;
    ShaderProperty _emissiveColorProp;
    
    ShaderProperty _baseTextureProp;
    ShaderProperty _normalTextureProp;
    ShaderProperty _emissiveTextureProp;
    ShaderProperty _occlusionTextureProp;
};

}

#endif /* pbr_base_material_hpp */
