//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef simple_mesh_utils_hpp
#define simple_mesh_utils_hpp

#include "simple_mesh.h"
#include <foundation/PxVec4.h>

namespace vox {
namespace cloth {
/*
 * returns a PxVec4 containing [x,y,z,d] for plane equation ax + by + cz + d = 0.
 * Where plane contains p and has normal n.
 */
inline physx::PxVec4 constructPlaneFromPointNormal(physx::PxVec3 p, physx::PxVec3 n) {
    n.normalize();
    return physx::PxVec4(n, -p.dot(n));
}

/*
 * returns two vectors in b and c so that [a b c] form a basis.
 * a needs to be a unit vector.
 */
inline void computeBasis(const physx::PxVec3 &a, physx::PxVec3 *b, physx::PxVec3 *c) {
    if (fabsf(a.x) >= 0.57735f)
        *b = physx::PxVec3(a.y, -a.x, 0.0f);
    else
        *b = physx::PxVec3(0.0f, a.z, -a.y);
    
    *b = b->getNormalized();
    *c = a.cross(*b);
}

struct Point {
    Point() {
    }
    
    Point(physx::PxVec3 _p) : p(_p) {
    }
    
    physx::PxVec3 p;
    
    Point operator*(float f) const {
        return Point(p * f);
    }
    
    Point operator+(Point pt) const {
        return Point(p + pt.p);
    }
};

struct RenderVertex {
    RenderVertex() {
    }
    
    RenderVertex(physx::PxVec3 _p, physx::PxVec3 _n) : p(_p), n(_n) {
    }
    
    physx::PxVec3 p;
    physx::PxVec3 n;
};

struct Polygon {
    Polygon() {
    }
    
    template<typename P, typename... Args>
    Polygon(P p1, P p2, P p3, Args... args) {
        addPoints(p1, p2, p3, args...);
    }
    
    std::vector<Point> mPoints;
    
    bool isTriangle() const {
        return mPoints.size() == 3;
    }
    
    template<typename P, typename... Args>
    void addPoints(P p, Args... args) {
        mPoints.push_back(p);
        addPoints(args...);
    }
    
    template<typename P>
    void addPoints(P p) {
        mPoints.push_back(p);
    }
    
    void triangulate(std::vector<Polygon> &out) const;
    
    void triangulate(std::vector<RenderVertex> &verts, std::vector<uint16_t> &indices) const;
    
    void triangulateWeld(std::vector<RenderVertex> &verts, std::vector<uint16_t> &indices) const; //normalize normals afterwards
    void triangulateForCollision(std::vector<physx::PxVec3> &verts) const;
    
    physx::PxVec3 calculateNormal() const;
    
    float calculateArea() const;
    
    void subdivideTriangle(std::vector<Polygon> &out) const;
    
    bool pointPlaneSide(physx::PxVec3 p, physx::PxVec4 plane) const;
    
    void clip(physx::PxVec4 plane, bool flip = false);
};

struct PolygonMesh {
    std::vector<Polygon> mPolygons;
    
    bool isTriangleMesh() const {
        bool b = true;
        for (const auto &p: mPolygons) {b = b && p.isTriangle();}
        return b;
    }
    
    void addConvexPolygon(physx::PxVec4 plane, physx::PxVec4 *planes, uint32_t mask, bool flip);
    
    void generateRenderBuffers(RenderVertex **vertices, uint16_t **indices, int *vertexCount, int *indexCount) const;
    
    void generateSmoothRenderBuffers(RenderVertex **vertices, uint16_t **indices, int *vertexCount, int *indexCount) const;
    
    int generateTriangleList(physx::PxVec3 **positions);
    
    void applyTransfom(physx::PxMat44 transform);
    
    void merge(const PolygonMesh &mesh);
};

PolygonMesh generateTetrahedron(float radius);

PolygonMesh generateIcosahedron(float radius, int subdivisions);

PolygonMesh generateCone(physx::PxVec4 a, physx::PxVec4 b, int segments, float grow, bool correctCone);

PolygonMesh generateCollisionConvex(physx::PxVec4 *planes, uint32_t mask, float grow, bool flip);

PolygonMesh generateCollisionCapsules(physx::PxVec4 *spheres, int sphereCount, uint32_t *indices, int indexCount, float grow);

//Generates simple meshes with smooth shading
SimpleMesh generateFastSphere(int segmentsX, int segmentY, physx::PxMat44 transform);
SimpleMesh generateFastCylinder(int segmentsX, int segmentY, physx::PxMat44 transform); //no caps

//Combines cashed spheres and cylinders to generate the capsules
SimpleMesh generateCollisionCapsulesFast(physx::PxVec4* spheres, int sphereCount,
                                         uint32_t* indices, int indexCount, float grow);

uint32_t generateConvexPolyhedronPlanes(int segmentsX, int segmentsY, physx::PxVec3 center,
                                        float radius, std::vector<physx::PxVec4> *planes);


}
}

#endif /* simple_mesh_utils_hpp */
