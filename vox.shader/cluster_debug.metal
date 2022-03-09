//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cluster_compute.h"
using namespace metal;

typedef struct {
    float4 position [[position]];
    float2 v_uv;
} VertexOut;

fragment float4 fragment_cluster_debug(VertexOut in [[stage_in]],
                                       device float4& u_cluster_uniform [[buffer(5)]],
                                       device ClusterLightGroup& u_clusterLights [[buffer(6)]]) {
    uint32_t clusterIndex = getClusterIndex(u_cluster_uniform, in.position);
    uint32_t lightCount = u_clusterLights.lights[clusterIndex].point_count + u_clusterLights.lights[clusterIndex].spot_count;
    float lightFactor = float(lightCount) / float(MAX_LIGHTS_PER_CLUSTER);
    
    return mix(float4(0.0, 0.0, 1.0, 1.0), float4(1.0, 0.0, 0.0, 1.0),
               float4(lightFactor, lightFactor, lightFactor, lightFactor));
}
