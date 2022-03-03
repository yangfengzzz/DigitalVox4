//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
using namespace metal;
#include "particle_perlin_2d.h"

void pnoise_gradients(float2 pt,
                      float2 scaledTileRes,
                      int uPerlinNoisePermutationSeed,
                      thread float4& gradients,
                      thread float4& fpt) {
    // Retrieve the integral part (for indexation)
    float4 ipt = floor(pt.xyxy) + float4(0.0f, 0.0f, 1.0f, 1.0f);
    
    // Tile the noise (if enabled)
#if NOISE_ENABLE_TILING
    ipt = mod(ipt, scaledTileRes.xyxy);
#endif
    ipt = mod289(ipt);
    
    // Compute the 4 corners hashed gradient indices
    float4 ix = ipt.xzxz;
    float4 iy = ipt.yyww;
    float4 p = permute(permute(ix, uPerlinNoisePermutationSeed) + iy, uPerlinNoisePermutationSeed);
    /*
     Fast version for :
     p.x = P(P(ipt.x)      + ipt.y);
     p.y = P(P(ipt.x+1.0f) + ipt.y);
     p.z = P(P(ipt.x)      + ipt.y+1.0f);
     p.w = P(P(ipt.x+1.0f) + ipt.y+1.0f);
     */
    
    // With 'p', computes Pseudo Random Numbers
    const float one_over_41 = 1.0f / 41.0f; //0.02439f
    float4 gx = 2.0f * fract(p * one_over_41) - 1.0f;
    float4 gy = abs(gx) - 0.5f;
    float4 tx = floor(gx + 0.5f);
    gx = gx - tx;
    
    // Create unnormalized gradients
    float2 g00 = float2(gx.x,gy.x);
    float2 g10 = float2(gx.y,gy.y);
    float2 g01 = float2(gx.z,gy.z);
    float2 g11 = float2(gx.w,gy.w);
    
    // 'Fast' normalization
    float4 dp = float4(dot(g00, g00), dot(g10, g10), dot(g01, g01), dot(g11, g11));
    float4 norm = rsqrt(dp);
    g00 *= norm.x;
    g10 *= norm.y;
    g01 *= norm.z;
    g11 *= norm.w;
    
    // Retrieve the fractional part (for interpolation)
    fpt = fract(pt.xyxy) - float4(0.0f, 0.0f, 1.0f, 1.0f);
    
    // Calculate gradient's influence
    float4 fx = fpt.xzxz;
    float4 fy = fpt.yyww;
    float n00 = dot(g00, float2(fx.x, fy.x));
    float n10 = dot(g10, float2(fx.y, fy.y));
    float n01 = dot(g01, float2(fx.z, fy.z));
    float n11 = dot(g11, float2(fx.w, fy.w));
    /*
     Fast version for :
     n00 = dot(g00, fpt + vec2(0.0f, 0.0f));
     n10 = dot(g10, fpt + vec2(-1.0f, 0.0f));
     n01 = dot(g01, fpt + vec2(0.0f,-1.0f));
     n11 = dot(g11, fpt + vec2(-1.0f,-1.0f));
     */
    
    gradients = float4(n00, n10, n01, n11);
}

void pnoise_gradients(float2 pt,
                      int uPerlinNoisePermutationSeed,
                      thread float4& gradients,
                      thread float4& fpt) {
    pnoise_gradients(pt, float2(0.0f), uPerlinNoisePermutationSeed,
                     gradients, fpt);
}

// Classical Perlin Noise 2D
float pnoise(float2 pt,
             float2 scaledTileRes,
             int uPerlinNoisePermutationSeed) {
    thread float4 g, fpt;
    pnoise_gradients(pt, scaledTileRes, uPerlinNoisePermutationSeed, g, fpt);
    
    // Interpolate gradients
    float2 u = fade(fpt.xy);
    float n1 = mix(g.x, g.y, u.x);
    float n2 = mix(g.z, g.w, u.x);
    float noise = mix(n1, n2, u.y);
    
    return noise;
}

float pnoise(float2 pt, int uPerlinNoisePermutationSeed) {
    return pnoise(pt, float2(0.0f), uPerlinNoisePermutationSeed);
}


// Derivative Perlin Noise 2D
float3 dpnoise(float2 pt, float2 scaledTileRes, int uPerlinNoisePermutationSeed) {
    thread float4 g, fpt;
    pnoise_gradients(pt, scaledTileRes, uPerlinNoisePermutationSeed, g, fpt);
    
    float k0 = g.x;
    float k1 = g.y - g.x;
    float k2 = g.z - g.x;
    float k3 = g.x - g.z - g.y + g.w;
    float3 res;
    
    float2 u = fade(fpt.xy);
    res.x = k0 + k1*u.x + k2*u.y + k3*u.x*u.y;
    
    float2 dpt = 30.0f*fpt.xy*fpt.xy*(fpt.xy*(fpt.xy-2.0f)+1.0f);
    res.y = dpt.x * (k1 + k3*u.y);
    res.z = dpt.y * (k2 + k3*u.x);
    
    return res;
}

float3 dpnoise(float2 pt, int uPerlinNoisePermutationSeed) {
    return dpnoise(pt, float2(0.0f), uPerlinNoisePermutationSeed);
}


//MARK: -- Fractional Brownian Motion function --
// Classical Perlin Noise fbm 2D
float fbm_pnoise(float2 pt,
                 const float zoom,
                 const int numOctave,
                 const float frequency,
                 const float amplitude,
                 int uPerlinNoisePermutationSeed) {
    float sum = 0.0f;
    float f = frequency;
    float w = amplitude;
    
    float2 v = zoom * pt;
    float2 scaledTileRes = zoom * NOISE_TILE_RES.xy;
    
    for (int i = 0; i < numOctave; ++i) {
        sum += w * pnoise(f*v, f*scaledTileRes, uPerlinNoisePermutationSeed);
        f *= frequency;
        w *= amplitude;
    }
    
    return sum;
}

// Derivative Perlin Noise fbm 2D
float fbm_dpnoise(float2 pt,
                  const float zoom,
                  const int numOctave,
                  const float frequency,
                  const float amplitude,
                  int uPerlinNoisePermutationSeed) {
    float sum = 0.0f;
    
    float f = frequency;
    float w = amplitude;
    float2 dn = float2(0.0f);
    
    float2 v = zoom * pt;
    float2 scaledTileRes = zoom * NOISE_TILE_RES.xy;
    
    for (int i=0; i<numOctave; ++i) {
        float3 n = dpnoise(f*v, f*scaledTileRes, uPerlinNoisePermutationSeed);
        dn += n.yz;
        
        float crestFactor = 1.0f / (1.0f + dot(dn,dn));
        
        sum += w * n.x * crestFactor;
        f *= frequency;
        w *= amplitude;
    }
    
    return sum;
}
