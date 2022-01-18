//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

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
