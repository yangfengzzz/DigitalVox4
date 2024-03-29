//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef function_constant_h
#define function_constant_h

#include <metal_stdlib>
using namespace metal;
#import "shader_common.h"
#include "macro_name.h"

constant bool hasUV [[function_constant(HAS_UV)]];
constant bool hasNormal [[function_constant(HAS_NORMAL)]];
constant bool hasTangent [[function_constant(HAS_TANGENT)]];
constant bool hasVertexColor [[function_constant(HAS_VERTEXCOLOR)]];
constant bool omitNormal [[function_constant(OMIT_NORMAL)]];
constant bool notOmitNormalAndHasNormal = !omitNormal && hasNormal;
constant bool notOmitNormalAndHasTangent = !omitNormal && hasTangent;

constant bool hasBlendShape [[function_constant(HAS_BLENDSHAPE)]];
constant bool hasBlendShapeNormal [[function_constant(HAS_BLENDSHAPE_NORMAL)]];
constant bool hasBlendShapeTangent [[function_constant(HAS_BLENDSHAPE_TANGENT)]];
constant bool hasBlendShapeAndHasBlendShapeNormal = hasBlendShape && hasBlendShapeNormal;
constant bool hasBlendShapeAndhasBlendShapeTangent = hasBlendShape && hasBlendShapeTangent;

constant bool hasSkin [[function_constant(HAS_SKIN)]];
constant bool hasJointTexture [[function_constant(HAS_JOINT_TEXTURE)]];
constant bool hasSkinAndHasJointTexture = hasSkin && hasJointTexture;
constant bool hasSkinNotHasJointTexture = hasSkin && !hasJointTexture;
constant int jointsCount [[function_constant(JOINTS_COUNT)]];

constant bool needAlphaCutoff [[function_constant(NEED_ALPHA_CUTOFF)]];
constant bool needWorldPos [[function_constant(NEED_WORLDPOS)]];
constant bool needTilingOffset [[function_constant(NEED_TILINGOFFSET)]];
constant bool hasDiffuseTexture [[function_constant(HAS_DIFFUSE_TEXTURE)]];
constant bool hasSpecularTexture [[function_constant(HAS_SPECULAR_TEXTURE)]];
constant bool hasEmissiveTexture [[function_constant(HAS_EMISSIVE_TEXTURE)]];
constant bool hasNormalTexture [[function_constant(HAS_NORMAL_TEXTURE)]];
constant bool hasNormalAndHasTangentAndHasNormalTexture = hasNormal && hasTangent && hasNormalTexture;
constant bool hasNormalNotHasTangentOrHasNormalTexture = hasNormal && (!hasTangent || !hasNormalTexture);

constant bool hasBaseTexture [[function_constant(HAS_BASE_TEXTURE)]];
constant bool hasBaseColorMap [[function_constant(HAS_BASE_COLORMAP)]];
constant bool hasEmissiveMap [[function_constant(HAS_EMISSIVEMAP)]];
constant bool hasOcclusionMap [[function_constant(HAS_OCCLUSIONMAP)]];
constant bool hasSpecularGlossinessMap [[function_constant(HAS_SPECULARGLOSSINESSMAP)]];
constant bool hasMetalRoughnessMap [[function_constant(HAS_METALROUGHNESSMAP)]];
constant bool isMetallicWorkFlow [[function_constant(IS_METALLIC_WORKFLOW)]];

constant int directLightCount [[function_constant(DIRECT_LIGHT_COUNT)]];
constant bool hasDirectLight = directLightCount > 0;
constant int pointLightCount [[function_constant(POINT_LIGHT_COUNT)]];
constant bool hasPointLight = pointLightCount > 0;
constant int spotLightCount [[function_constant(SPOT_LIGHT_COUNT)]];
constant bool hasSpotLight = spotLightCount > 0;
constant bool needForwardPlus [[function_constant(NEED_FORWARD_PLUS)]];

constant bool hasSH [[function_constant(HAS_SH)]];
constant bool hasSpecularEnv [[function_constant(HAS_SPECULAR_ENV)]];
constant bool hasDiffuseEnv [[function_constant(HAS_DIFFUSE_ENV)]];

constant bool needParticleScattering [[function_constant(NEED_PARTICLE_SCATTERING)]];
constant bool needParticleVectorField [[function_constant(NEED_PARTICLE_VECTOR_FIELD)]];
constant bool needParticleCurlNoise [[function_constant(NEED_PARTICLE_CURL_NOISE)]];
constant bool needParticleVelocityControl [[function_constant(NEED_PARTICLE_VELOCITY_CONTROL)]];

constant int shadowMapCount [[function_constant(SHADOW_MAP_COUNT)]];
constant int cubeShadowMapCount [[function_constant(CUBE_SHADOW_MAP_COUNT)]];
constant bool hasShadow = shadowMapCount > 0;
constant bool hasCubeShadow = cubeShadowMapCount > 0;

//MARK: - Function Signature
float4x4 getJointMatrix(sampler smp, texture2d<float> joint_tex,
                        float index, int u_jointCount);

#endif // function_constant_h
