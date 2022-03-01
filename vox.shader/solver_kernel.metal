//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <metal_stdlib>
#include "cloth_data.h"
using namespace metal;

#ifndef FLT_MAX
#define FLT_MAX 3.402823466e+38F
#endif

struct IndexPair {
    uint32_t first;
    uint32_t second;
};

constant const uint32_t blockDim = 1024;
constant const uint32_t BlockSize = blockDim;
constant const uint32_t WarpsPerBlock = (BlockSize >> 5);

#define FLT_EPSILON 1.192092896e-07F

//interface IParticles {
//    float4 get(uint32_t index);
//    void set(uint32_t index, float4 value);
//    void atomicAdd(uint32_t index, float3 value);
//};

template<class IParticles>
void integrateParticles(IParticles curParticles, IParticles prevParticles, uint32_t threadIdx,
                        threadgroup DxClothData& gClothData, threadgroup DxIterationData& gIterData) {
    for (uint32_t i = threadIdx; i < gClothData.mNumParticles; i += blockDim) {
        float4 curPos = curParticles.get(i);
        
        float nextX = curPos.x, curX = nextX;
        float nextY = curPos.y, curY = nextY;
        float nextZ = curPos.z, curZ = nextZ;
        float nextW = curPos.w;
        
        float4 prevPos = prevParticles.get(i);
        
        if (nextW == 0.0f)
            nextW = prevPos.w;
        
        if (nextW > 0.0f) {
            float prevX = prevPos.x;
            float prevY = prevPos.y;
            float prevZ = prevPos.z;
            
            if (gIterData.mIsTurning) {
                nextX = nextX + gIterData.mIntegrationTrafo[3] + curX * gIterData.mIntegrationTrafo[15] +
                prevX * gIterData.mIntegrationTrafo[6] + curY * gIterData.mIntegrationTrafo[16] +
                prevY * gIterData.mIntegrationTrafo[7] + curZ * gIterData.mIntegrationTrafo[17] +
                prevZ * gIterData.mIntegrationTrafo[8];
                
                nextY = nextY + gIterData.mIntegrationTrafo[4] + curX * gIterData.mIntegrationTrafo[18] +
                prevX * gIterData.mIntegrationTrafo[9] + curY * gIterData.mIntegrationTrafo[19] +
                prevY * gIterData.mIntegrationTrafo[10] + curZ * gIterData.mIntegrationTrafo[20] +
                prevZ * gIterData.mIntegrationTrafo[11];
                
                nextZ = nextZ + gIterData.mIntegrationTrafo[5] + curX * gIterData.mIntegrationTrafo[21] +
                prevX * gIterData.mIntegrationTrafo[12] + curY * gIterData.mIntegrationTrafo[22] +
                prevY * gIterData.mIntegrationTrafo[13] + curZ * gIterData.mIntegrationTrafo[23] +
                prevZ * gIterData.mIntegrationTrafo[14];
            }
            else {
                nextX += (curX - prevX) * gIterData.mIntegrationTrafo[6] + gIterData.mIntegrationTrafo[3];
                nextY += (curY - prevY) * gIterData.mIntegrationTrafo[9] + gIterData.mIntegrationTrafo[4];
                nextZ += (curZ - prevZ) * gIterData.mIntegrationTrafo[12] + gIterData.mIntegrationTrafo[5];
            }
            
            curX += gIterData.mIntegrationTrafo[0];
            curY += gIterData.mIntegrationTrafo[1];
            curZ += gIterData.mIntegrationTrafo[2];
        }
        
        curPos.x = nextX;
        curPos.y = nextY;
        curPos.z = nextZ;
        curPos.w = nextW;
        curParticles.set(i, curPos);
        
        prevPos.x = curX;
        prevPos.y = curY;
        prevPos.z = curZ;
        prevParticles.set(i, prevPos);
    }
}

kernel void cloth_solver(device float4* bParticles,
                         device float4* bSelfCollisionParticles,
                         device float4* bSelfCollisionData,
                         
                         device DxFrameData* bFrameData,
                         
                         device DxClothData* bClothData,
                         device DxIterationData* bIterData,
                         
                         device DxPhaseConfig* bPhaseConfigs,
                         device DxConstraint* bConstraints,
                         device DxTether* bTetherConstraints,
                         
                         device IndexPair* bCapsuleIndices,
                         device float4* bCollisionSpheres,

                         device uint32_t* bConvexMasks,
                         device float4* bCollisionPlanes,

                         device float3* bCollisionTriangles,

                         device float4* bMotionConstraints,
                         device float4* bSeparationConstraints,

                         device float4* bParticleAccelerations,

                         device float4* bRestPositions,

                         device int32_t* bSelfCollisionIndices,

                         device float* bPerConstraintStiffness,

                         //cloth mesh triangle information for air drag/lift
                         //Note that the buffer actually contains uint16_t values
                         device uint32_t* bTriangles,

                         device int32_t* bVirtualParticleSetSizes,
                         device uint4* bVirtualParticleIndices,
                         device float4* bVirtualParticleWeights,

                         uint32_t blockIdx [[ simdgroup_index_in_threadgroup ]],
                         uint32_t threadIdx [[ thread_index_in_simdgroup ]]) {
    threadgroup DxClothData gClothData;
    threadgroup DxFrameData gFrameData;
    threadgroup DxIterationData gIterData;
    threadgroup uint gCurParticles[MaxParticlesInSharedMem * 4];

    threadgroup float gBounds[192];
    
    if (!threadIdx) {
        gClothData = bClothData[blockIdx];
        gFrameData = bFrameData[blockIdx];
    }
    simdgroup_barrier(mem_flags::mem_threadgroup); // wait for gClothData being written
    
    if (!threadIdx) {
        bFrameData[blockIdx] = gFrameData;
    }
}

