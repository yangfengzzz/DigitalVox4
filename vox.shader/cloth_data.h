//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cloth_data_h
#define cloth_data_h

typedef unsigned int uint32_t;
typedef int int32_t;

constant const uint32_t MaxParticlesInSharedMem = 1969;

struct DxPhaseConfig {
    float mStiffness;
    float mStiffnessMultiplier;
    float mCompressionLimit;
    float mStretchLimit;
    
    uint32_t mFirstConstraint;
    uint32_t mNumConstraints;
};

struct DxConstraint {
    float mRestvalue;
    uint32_t mIndices; // 2x uint16_t
};

struct DxTether {
    uint32_t mValue;   // 2x uint16_t
};

// reference to cloth instance bulk data (POD)
// should not need frequent updates (stored on device)
struct DxClothData {
    uint32_t mNumParticles;
    uint32_t mParticlesOffset;
    
    // fabric constraints
    uint32_t mNumPhases;
    uint32_t mPhaseConfigOffset;
    uint32_t mConstraintOffset;
    uint32_t mStiffnessOffset; //Offset inside per constraint stiffness buffer
    
    uint32_t mNumTethers;
    uint32_t mTetherOffset;
    float mTetherConstraintScale;
    
    uint32_t mNumTriangles;
    uint32_t mStartTriangleOffset;
    
    // motion constraint data
    float mMotionConstraintScale;
    float mMotionConstraintBias;
    
    // collision
    uint32_t mNumCapsules;
    uint32_t mCapsuleOffset;
    uint32_t mNumSpheres;
    
    uint32_t mNumPlanes;
    uint32_t mNumConvexes;
    uint32_t mConvexMasksOffset;
    
    uint32_t mNumCollisionTriangles;
    
    uint32_t mNumVirtualParticleSetSizes;
    
    uint32_t mEnableContinuousCollision; //bool stored in uint32_t for dx alignment
    float mCollisionMassScale;
    float mFrictionScale;
    
    float mSelfCollisionDistance;
    
    uint32_t mNumSelfCollisionIndices;
    uint32_t mSelfCollisionIndicesOffset;
    uint32_t mSelfCollisionParticlesOffset;
    uint32_t mSelfCollisionDataOffset;
    
    // sleep data
    uint32_t mSleepTestInterval;
    uint32_t mSleepAfterCount;
    float mSleepThreshold;
};

// per-frame data (stored in pinned memory)
struct DxFrameData {
    bool mDeviceParticlesDirty;
    
    // number of particle copies that fit in shared memory (0, 1, or 2)
    uint32_t mNumSharedPositions;
    
    // iteration data
    float mIterDt;
    uint32_t mFirstIteration;
    uint32_t mNumIterations;
    
    float mTetherConstraintStiffness;
    
    // wind data
    float mDragCoefficient;
    float mLiftCoefficient;
    float mFluidDensity;
    float mRotation[9];
    
    // motion constraint data
    float mMotionConstraintStiffness;
    uint32_t mStartMotionConstrainsOffset;
    uint32_t mTargetMotionConstrainsOffset;
    
    // separation constraint data
    uint32_t mStartSeparationConstrainsOffset;
    uint32_t mTargetSeparationConstrainsOffset;
    
    // particle acceleration data
    uint32_t mParticleAccelerationsOffset;
    
    uint32_t mStartSphereOffset;
    uint32_t mTargetSphereOffset;
    
    uint32_t mStartCollisionPlaneOffset;
    uint32_t mTargetCollisionPlaneOffset;
    
    uint32_t mStartCollisionTrianglesOffset;
    uint32_t mTargetCollisionTrianglesOffset;
    
    float mSelfCollisionStiffness;
    
    float mParticleBounds[6]; // maxX, -minX, maxY, ...
    
    uint32_t mSleepPassCounter;
    uint32_t mSleepTestCounter;
    
    float mStiffnessExponent;
    
    uint32_t mRestPositionsOffset;
    
    bool mInitSelfCollisionData;
};

// per-iteration data (stored in pinned memory)
struct DxIterationData {
    float mIntegrationTrafo[24];
    float mWind[3];
    uint32_t mIsTurning;
};

#endif /* cloth_data_h */
