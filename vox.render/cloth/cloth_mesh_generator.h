//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cloth_mesh_generator_hpp
#define cloth_mesh_generator_hpp

#include <foundation/PxVec3.h>
#include <foundation/PxVec2.h>
#include <NvClothExt/ClothFabricCooker.h>
#include <foundation/PxMat44.h>
#include "cloth/simple_mesh.h"

namespace vox {
namespace cloth {

struct ClothMeshData {
    template<typename T>
    static nv::cloth::BoundedData ToBoundedData(T &vector) {
        nv::cloth::BoundedData d;
        d.data = &vector[0];
        d.stride = sizeof(vector[0]);
        d.count = (physx::PxU32) vector.size();
        
        return d;
    }
    
    struct Triangle {
        Triangle() {
        }
        
        Triangle(uint32_t _a, uint32_t _b, uint32_t _c) :
        a(_a), b(_b), c(_c) {
        }
        
        uint32_t a, b, c;
        
        Triangle operator+(uint32_t offset) const {
            return Triangle(a + offset, b + offset, c + offset);
        }
    };
    
    struct Quad {
        Quad() {
        }
        
        Quad(uint32_t _a, uint32_t _b, uint32_t _c, uint32_t _d) :
        a(_a), b(_b), c(_c), d(_d) {
        }
        
        uint32_t a, b, c, d;
        
        Quad operator+(uint32_t offset) const {
            return Quad(a + offset, b + offset, c + offset, d + offset);
        }
    };
    
    std::vector<physx::PxVec3> mVertices;
    std::vector<physx::PxVec2> mUvs;
    std::vector<Triangle> mTriangles;
    std::vector<Quad> mQuads;
    std::vector<physx::PxReal> mInvMasses;
    
    SimpleMesh mMesh;
    
    void Clear();
    
    void GeneratePlaneCloth(float width, float height, int segmentsX, int segmentsY,
                            bool createQuads = false, physx::PxMat44 transform = physx::PxIdentity,
                            bool alternatingDiagonals = true, int zigzag = 0);
    
    void GenerateCylinderWave(float radiusTop, float radiusBottom, float height,
                              float frequency, float ampitudeTop, float ampitudeBottom, int segmentsX, int segmentsY,
                              physx::PxMat44 transform = physx::PxIdentity,
                              bool attachTop = false, bool attachBottom = false,
                              bool createQuads = false, int missingXsegments = 0);
    
    void AttachClothPlaneByAngles(int segmentsX, int segmentsY, bool attachByWidth = true);
    
    void AttachClothPlaneBySide(int segmentsX, int segmentsY, bool attachByWidth = true);
    
    bool ReadClothFromFile(const std::string &verticesPath, const std::string &indicesPath,
                           physx::PxMat44 transform = physx::PxIdentity);
    
    //positions as float (3 elements per position)
    template<typename PositionType = float, typename IndexType = uint16_t>
    bool InitializeFromData(nv::cloth::BoundedData positions, nv::cloth::BoundedData indices,
                            physx::PxMat44 transform = physx::PxMat44(physx::PxIdentity));
    
    void AttachClothUsingTopVertices(float thresholdY = 0.5f);
    
    void SetInvMasses(float invMass);
    
    void SetInvMassesFromDensity(float density);     // Todo
    
    nv::cloth::ClothMeshDesc GetClothMeshDesc();
    
    SimpleMesh GetRenderMesh();
    
    void Merge(const ClothMeshData &other);
};


}
}

#endif /* cloth_mesh_generator_hpp */
