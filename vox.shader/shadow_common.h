//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef shadow_common_h
#define shadow_common_h

#include <metal_stdlib>
using namespace metal;
#include "function_constant.h"

float textureProj(float3 worldPos, float3 viewPos, float2 off,
                  depth2d_array<float> u_shadowMap,
                  constant ShadowData* u_shadowData,
                  int index);

float cubeTextureProj(float3 worldPos, float3 viewPos, float2 off,
                      depthcube_array<float> u_shadowMap,
                      constant CubeShadowData* u_shadowData,
                      int index);

float filterPCF(float3 worldPos, float3 viewPos,
                depth2d_array<float> u_shadowMap,
                constant ShadowData* u_shadowData,
                int index);

float cubeFilterPCF(float3 worldPos, float3 viewPos,
                    depthcube_array<float> u_shadowMap,
                    constant CubeShadowData* u_shadowData,
                    int index);

#endif /* shadow_common_h */
