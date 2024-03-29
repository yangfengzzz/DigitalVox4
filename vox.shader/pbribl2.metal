//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;
#import "function_constant.h"

struct VertexIn {
    float3 position [[attribute(Position)]];
    float3 normal [[attribute(Normal)]];
    float2 uv [[attribute(UV_0)]];
};

struct VertexOut {
    float4 position [[position]];
    float3 normal;
    float3 worldPos;
    float2 uv;
};

vertex VertexOut vertex_experimental(const VertexIn in [[stage_in]],
                                     constant matrix_float4x4 &u_localMat [[buffer(0)]],
                                     constant matrix_float4x4 &u_modelMat [[buffer(1)]],
                                     constant matrix_float4x4 &u_viewMat [[buffer(2)]],
                                     constant matrix_float4x4 &u_projMat [[buffer(3)]],
                                     constant matrix_float4x4 &u_MVMat [[buffer(4)]],
                                     constant matrix_float4x4 &u_MVPMat [[buffer(5)]],
                                     constant matrix_float4x4 &u_normalMat [[buffer(6)]],
                                     constant float3 &u_cameraPos [[buffer(7)]],
                                     constant float4 &u_tilingOffset [[buffer(8)]],
                                     device matrix_float4x4 *u_jointMatrix [[buffer(12), function_constant(hasSkinNotHasJointTexture)]]) {
    VertexOut out;
    out.worldPos = (u_modelMat * float4(in.position, 1.0)).xyz;
    out.normal = (u_normalMat * float4(in.normal, 0.0)).xyz;
    out.uv = in.uv;
    out.position = u_MVPMat * float4(in.position, 1.0);
    
    return out;
}

typedef struct Lighting {
    float3 lightDirection;
    float3 viewDirection;
    float3 baseColor;
    float3 normal;
    float metallic;
    float roughness;
    float ambientOcclusion;
    float3 lightColor;
} Lighting;

float3 render(Lighting lighting) {
    // Rendering equation courtesy of Apple et al.
    float nDotl = max(0.001, saturate(dot(lighting.normal, lighting.lightDirection)));
    float3 halfVector = normalize(lighting.lightDirection + lighting.viewDirection);
    float nDoth = max(0.001, saturate(dot(lighting.normal, halfVector)));
    float nDotv = max(0.001, saturate(dot(lighting.normal, lighting.viewDirection)));
    float hDotl = max(0.001, saturate(dot(lighting.lightDirection, halfVector)));
    
    // specular roughness
    float specularRoughness = lighting.roughness * (1.0 - lighting.metallic) + lighting.metallic;
    
    // Distribution
    float Ds;
    if (specularRoughness >= 1.0) {
        Ds = 1.0 / M_PI_F;
    }
    else {
        float roughnessSqr = specularRoughness * specularRoughness;
        float d = (nDoth * roughnessSqr - nDoth) * nDoth + 1;
        Ds = roughnessSqr / (M_PI_F * d * d);
    }
    
    // Fresnel
    float3 Cspec0 = float3(1.0);
    float fresnel = pow(clamp(1.0 - hDotl, 0.0, 1.0), 5.0);
    float3 Fs = float3(mix(float3(Cspec0), float3(1), fresnel));
    
    
    // Geometry
    float alphaG = (specularRoughness * 0.5 + 0.5) * (specularRoughness * 0.5 + 0.5);
    float a = alphaG * alphaG;
    float b1 = nDotl * nDotl;
    float b2 = nDotv * nDotv;
    float G1 = (float)(1.0 / (b1 + sqrt(a + b1 - a*b1)));
    float G2 = (float)(1.0 / (b2 + sqrt(a + b2 - a*b2)));
    float Gs = G1 * G2;
    
    float3 specularOutput = (Ds * Gs * Fs * lighting.lightColor) * (1.0 + lighting.metallic * lighting.baseColor) + lighting.metallic * lighting.lightColor * lighting.baseColor;
    specularOutput = specularOutput * lighting.ambientOcclusion;
    
    return specularOutput;
}

fragment float4 fragment_experimental(VertexOut in [[stage_in]],
                                      // common_frag
                                      constant matrix_float4x4 &u_localMat [[buffer(0)]],
                                      constant matrix_float4x4 &u_modelMat [[buffer(1)]],
                                      constant matrix_float4x4 &u_viewMat [[buffer(2)]],
                                      constant matrix_float4x4 &u_projMat [[buffer(3)]],
                                      constant matrix_float4x4 &u_MVMat [[buffer(4)]],
                                      constant matrix_float4x4 &u_MVPMat [[buffer(5)]],
                                      constant matrix_float4x4 &u_normalMat [[buffer(6)]],
                                      constant float3 &u_cameraPos [[buffer(7)]],
                                      constant EnvMapLight &u_envMapLight [[buffer(8)]],
                                      constant float3 *u_env_sh [[buffer(9), function_constant(hasSH)]],
                                      texturecube<float> u_env_specularTexture [[texture(0), function_constant(hasSpecularEnv)]],
                                      texturecube<float> u_env_diffuseTexture [[texture(1), function_constant(hasDiffuseEnv)]],
                                      texture2d<float> samplerBRDFLUT [[texture(2)]],
                                      // direct_light_frag
                                      constant float3 &u_directLightColor [[buffer(10), function_constant(hasDirectLight)]],
                                      constant float3 &u_directLightDirection [[buffer(11), function_constant(hasDirectLight)]],
                                      // point_light_frag
                                      constant float3 &u_pointLightColor [[buffer(12), function_constant(hasPointLight)]],
                                      constant float3 &u_pointLightPosition [[buffer(13), function_constant(hasPointLight)]],
                                      constant float &u_pointLightDistance [[buffer(14), function_constant(hasPointLight)]],
                                      // spot_light_frag
                                      constant float3 &u_spotLightColor [[buffer(15), function_constant(hasSpotLight)]],
                                      constant float3 &u_spotLightPosition [[buffer(16), function_constant(hasSpotLight)]],
                                      constant float3 &u_spotLightDirection [[buffer(17), function_constant(hasSpotLight)]],
                                      constant float &u_spotLightDistance [[buffer(18), function_constant(hasSpotLight)]],
                                      constant float &u_spotLightAngleCos [[buffer(19), function_constant(hasSpotLight)]],
                                      constant float &u_spotLightPenumbraCos [[buffer(20), function_constant(hasSpotLight)]],
                                      //pbr base frag define
                                      constant float &u_alphaCutoff [[buffer(21)]],
                                      constant float4 &u_baseColor [[buffer(22)]],
                                      constant float &u_metal [[buffer(23)]],
                                      constant float &u_roughness [[buffer(24)]],
                                      constant float3 &u_specularColor [[buffer(25)]],
                                      constant float &u_glossinessFactor [[buffer(26)]],
                                      constant float3 &u_emissiveColor [[buffer(27)]],
                                      constant float &u_normalIntensity [[buffer(28)]],
                                      constant float &u_occlusionStrength [[buffer(29)]],
                                      // pbr_texture_frag_define
                                      texture2d<float> u_baseColorTexture [[texture(3), function_constant(hasBaseColorMap)]],
                                      texture2d<float> u_normalTexture [[texture(4), function_constant(hasNormalTexture)]],
                                      texture2d<float> u_emissiveTexture [[texture(5), function_constant(hasEmissiveMap)]],
                                      texture2d<float> u_metallicRoughnessTexture [[texture(6), function_constant(hasMetalRoughnessMap)]],
                                      texture2d<float> u_specularTexture [[texture(7), function_constant(hasSpecularMap)]],
                                      texture2d<float> u_glossinessTexture [[texture(8), function_constant(hasGlossinessMap)]],
                                      texture2d<float> u_occlusionTexture [[texture(79), function_constant(hasOcclusionMap)]]) {
    constexpr sampler textureSampler(coord::normalized, filter::linear,
                                     address::repeat, compare_func:: less);
    
    // extract color
    float3 baseColor;
    if (hasBaseColorMap) {
        baseColor = u_baseColorTexture.sample(textureSampler, in.uv).rgb;
    } else {
        baseColor = u_baseColor.rgb;
    }
    // extract metallic
    float metallic;
    if (hasMetalRoughnessMap) {
        metallic = u_metallicRoughnessTexture.sample(textureSampler, in.uv).r;
    } else {
        metallic = u_metal;
    }
    // extract roughness
    float roughness;
    if (hasMetalRoughnessMap) {
        roughness = u_metallicRoughnessTexture.sample(textureSampler, in.uv).g;
    } else {
        roughness = u_roughness;
    }
    // extract ambient occlusion
    float ambientOcclusion;
    if (hasOcclusionMap) {
        ambientOcclusion = u_occlusionTexture.sample(textureSampler, in.uv).r;
    } else {
        ambientOcclusion = 1.0;
    }
    
    // normal map
    float3 normal = in.normal;
    normal = normalize(normal);
    
    float4 diffuse = u_env_diffuseTexture.sample(textureSampler, normal);
    diffuse = mix(pow(diffuse, 0.5), diffuse, metallic);
    
    float3 viewDirection = in.worldPos.xyz - u_cameraPos;
    float3 textureCoordinates = reflect(viewDirection, normal);
    
    constexpr sampler s(filter::linear, mip_filter::linear);
    float3 prefilteredColor = u_env_specularTexture.sample(s, textureCoordinates,
                                                           level(roughness * 10)).rgb;
    
    float nDotV = saturate(dot(normal, normalize(-viewDirection)));
    float2 envBRDF = samplerBRDFLUT.sample(s, float2(roughness, nDotV)).rg;
    
    float3 f0 = mix(0.04, baseColor.rgb, metallic);
    float3 specularIBL = f0 * envBRDF.r + envBRDF.g;
    
    float3 specular = prefilteredColor * specularIBL;
    float4 iblColor = diffuse * float4(baseColor, 1) + float4(specular, 1);
    iblColor *= ambientOcclusion;
    
    // all the necessary components are in place
    Lighting lighting;
    lighting.lightDirection = u_directLightDirection[0];
    lighting.viewDirection = viewDirection;
    lighting.baseColor = baseColor;
    lighting.normal = normal;
    lighting.metallic = metallic;
    lighting.roughness = roughness;
    lighting.ambientOcclusion = ambientOcclusion;
    lighting.lightColor = u_directLightColor[0];
    
    float3 directSpecular = render(lighting);
    
    // compute Lambertian diffuse
    float nDotl = max(0.001, saturate(dot(lighting.normal, lighting.lightDirection)));
    float3 directDiffuse = u_directLightColor[0] * baseColor * nDotl * ambientOcclusion;
    directDiffuse *= metallic;
    
    float4 emissiveMapColor = float4(0.0);
    if (hasEmissiveMap) {
        emissiveMapColor = u_emissiveTexture.sample(textureSampler, in.uv);
        emissiveMapColor = clamp(emissiveMapColor, float4(0, 0, 0, 0), float4(1,1,1,1));
    }
    
    return iblColor + float4(directSpecular + directDiffuse, 1.0) + emissiveMapColor;
}
