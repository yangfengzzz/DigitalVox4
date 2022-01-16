//
//  skybox_material.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/15.
//

#ifndef skybox_material_hpp
#define skybox_material_hpp

#include "material/material.h"
#include "vector2.h"

namespace vox {
/**
 * SkyboxMaterial
 */
class SkyBoxMaterial : public Material {
public:
    /**
     * Whether to decode from texture with RGBM format.
     */
    bool textureDecodeRGBM();
    
    void setTextureDecodeRGBM(bool value);
    
    /**
     * RGBM decode factor, default 5.0.
     */
    float RGBMDecodeFactor();
    
    void setRGBMDecodeFactor(float value);
    
    /**
     * Texture cube map of the sky box material.
     */
    std::shared_ptr<MTL::Texture> textureCubeMap();
    
    void setTextureCubeMap(std::shared_ptr<MTL::Texture> v);
    
    SkyBoxMaterial();
    
private:
    ShaderProperty _skyboxTextureProp;
    ShaderProperty _mvpNoscaleProp;
    
    Vector2F _decodeParam = Vector2F(0, 5);
};

}

#endif /* skybox_material_hpp */
