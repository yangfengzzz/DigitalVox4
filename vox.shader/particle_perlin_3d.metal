//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;
#include "particle_perlin_3d.h"

// Classical Perlin Noise 3D
float pnoise(float3 pt, float3 scaledTileRes,
             int uPerlinNoisePermutationSeed) {
    // Retrieve the integral part (for indexation)
    float3 ipt0 = floor(pt);
    float3 ipt1 = ipt0 + float3(1.0f);
    
    // Tile the noise
#if NOISE_ENABLE_TILING
    ipt0 = fmod(ipt0, scaledTileRes);
    ipt1 = fmod(ipt1, scaledTileRes);
#endif
    
    ipt0 = mod289(ipt0);
    ipt1 = mod289(ipt1);
    
    // Compute the 8 corners hashed gradient indices
    float4 ix = float4(ipt0.x, ipt1.x, ipt0.x, ipt1.x);
    float4 iy = float4(ipt0.yy, ipt1.yy);
    float4 p = permute(permute(ix, uPerlinNoisePermutationSeed) + iy, uPerlinNoisePermutationSeed);
    float4 p0 = permute(p + ipt0.zzzz, uPerlinNoisePermutationSeed);
    float4 p1 = permute(p + ipt1.zzzz, uPerlinNoisePermutationSeed);
    
    // Compute Pseudo Random Numbers
    float4 gx0 = p0 * (1.0f / 7.0f);
    float4 gy0 = fract(floor(gx0) * (1.0f / 7.0f)) - 0.5f;
    gx0 = fract(gx0);
    float4 gz0 = float4(0.5f) - abs(gx0) - abs(gy0);
    float4 sz0 = step(gz0, float4(0.0f));
    gx0 -= sz0 * (step(0.0f, gx0) - 0.5f);
    gy0 -= sz0 * (step(0.0f, gy0) - 0.5f);
    
    float4 gx1 = p1 * (1.0f / 7.0f);
    float4 gy1 = fract(floor(gx1) * (1.0f / 7.0f)) - 0.5f;
    gx1 = fract(gx1);
    float4 gz1 = float4(0.5f) - abs(gx1) - abs(gy1);
    float4 sz1 = step(gz1, float4(0.0f));
    gx1 -= sz1 * (step(0.0f, gx1) - 0.5f);
    gy1 -= sz1 * (step(0.0f, gy1) - 0.5f);
    
    
    // Create unnormalized gradients
    float3 g000 = float3(gx0.x, gy0.x, gz0.x);
    float3 g100 = float3(gx0.y, gy0.y, gz0.y);
    float3 g010 = float3(gx0.z, gy0.z, gz0.z);
    float3 g110 = float3(gx0.w, gy0.w, gz0.w);
    float3 g001 = float3(gx1.x, gy1.x, gz1.x);
    float3 g101 = float3(gx1.y, gy1.y, gz1.y);
    float3 g011 = float3(gx1.z, gy1.z, gz1.z);
    float3 g111 = float3(gx1.w, gy1.w, gz1.w);
    
    // 'Fast' normalization
    float4 dp = float4(dot(g000, g000), dot(g100, g100), dot(g010, g010), dot(g110, g110));
    float4 norm = rsqrt(dp);
    g000 *= norm.x;
    g100 *= norm.y;
    g010 *= norm.z;
    g110 *= norm.w;
    
    dp = float4(dot(g001, g001), dot(g101, g101), dot(g011, g011), dot(g111, g111));
    norm = rsqrt(dp);
    g001 *= norm.x;
    g101 *= norm.y;
    g011 *= norm.z;
    g111 *= norm.w;
    
    // Retrieve the fractional part (for interpolation)
    float3 fpt0 = fract(pt);
    float3 fpt1 = fpt0 - float3(1.0f);
    
    // Calculate gradient's influence
    float n000 = dot(g000, fpt0);
    float n100 = dot(g100, float3(fpt1.x, fpt0.yz));
    float n010 = dot(g010, float3(fpt0.x, fpt1.y, fpt0.z));
    float n110 = dot(g110, float3(fpt1.xy, fpt0.z));
    float n001 = dot(g001, float3(fpt0.xy, fpt1.z));
    float n101 = dot(g101, float3(fpt1.x, fpt0.y, fpt1.z));
    float n011 = dot(g011, float3(fpt0.x, fpt1.yz));
    float n111 = dot(g111, fpt1);
    
    // Interpolate gradients
    float3 u = fade(fpt0);
    float nxy0 = mix(mix(n000, n100, u.x), mix(n010, n110, u.x), u.y);
    float nxy1 = mix(mix(n001, n101, u.x), mix(n011, n111, u.x), u.y);
    float noise = mix(nxy0, nxy1, u.z);
    
    return noise;
}

float pnoise(float3 pt, int uPerlinNoisePermutationSeed) {
    return pnoise(pt, float3(0.0f), uPerlinNoisePermutationSeed);
}

// Classical Perlin Noise 2D + time
float pnoise_loop(float2 u, float dt, int uPerlinNoisePermutationSeed) {
    float3 pt1 = float3(u, dt);
    float3 pt2 = float3(u, dt-1.0f);
    
    return mix(pnoise(pt1, uPerlinNoisePermutationSeed), pnoise(pt2, uPerlinNoisePermutationSeed), dt);
}

// Classical Perlin Noise fbm 3D
float fbm_pnoise(float3 pt,
                 const float zoom,
                 const int numOctave,
                 const float frequency,
                 const float amplitude,
                 int uPerlinNoisePermutationSeed) {
    float sum = 0.0f;
    float f = frequency;
    float w = amplitude;
    
    float3 v = zoom * pt;
    float3 scaledTileRes = zoom * NOISE_TILE_RES;
    
    for (int i = 0; i < numOctave; ++i) {
        sum += w * pnoise(f*v, f*scaledTileRes, uPerlinNoisePermutationSeed);
        
        f *= frequency;
        w *= amplitude;
    }
    
    return sum;
}

float fbm_3d(float3 ws, int uPerlinNoisePermutationSeed) {
    const float N = 128.0f;
    const float zoom = 1/N;
    const int octave = 4;
    const float freq = 2.0f;
    const float w    = 0.45f;
    
    return N * fbm_pnoise(ws, zoom, octave, freq, w, uPerlinNoisePermutationSeed);
}

