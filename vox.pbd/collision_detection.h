//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef collision_detection_hpp
#define collision_detection_hpp

#include "vector3.h"
#include "bounding_box3.h"
#include <vector>

namespace vox {
namespace pbd {

class CollisionDetection {
public:
    static const unsigned int RigidBodyContactType;          // = 0;
    static const unsigned int ParticleContactType;           // = 1;
    static const unsigned int ParticleRigidBodyContactType;  // = 2;
    static const unsigned int ParticleSolidContactType;      // = 3;
    
    typedef void (*ContactCallbackFunction)(const unsigned int contactType, const unsigned int bodyIndex1, const unsigned int bodyIndex2,
                                            const Vector3F &cp1, const Vector3F &cp2,
                                            const Vector3F &normal, const float dist,
                                            const float restitutionCoeff, const float frictionCoeff, void *userData);
    
    typedef void (*SolidContactCallbackFunction)(const unsigned int contactType, const unsigned int bodyIndex1, const unsigned int bodyIndex2,
                                                 const unsigned int tetIndex, const Vector3F &bary,
                                                 const Vector3F &cp1, const Vector3F &cp2,
                                                 const Vector3F &normal, const float dist,
                                                 const float restitutionCoeff, const float frictionCoeff, void *userData);
    
    struct CollisionObject {
        static const unsigned int RigidBodyCollisionObjectType;     // = 0;
        static const unsigned int TriangleModelCollisionObjectType; // = 1;
        static const unsigned int TetModelCollisionObjectType;      // = 2;
        
        BoundingBox3F m_aabb;
        unsigned int m_bodyIndex;
        unsigned int m_bodyType;
        
        virtual ~CollisionObject() {
        }
        
        virtual int &getTypeId() const = 0;
    };
    
    struct CollisionObjectWithoutGeometry : public CollisionObject {
        static int TYPE_ID;
        
        virtual int &getTypeId() const {
            return TYPE_ID;
        }
        
        virtual ~CollisionObjectWithoutGeometry() {
        }
    };
    
protected:
    float m_tolerance;
    ContactCallbackFunction m_contactCB;
    SolidContactCallbackFunction m_solidContactCB;
    void *m_contactCBUserData;
    void *m_solidContactCBUserData;
    std::vector<CollisionObject *> m_collisionObjects;
    
    void updateAABB(const Vector3F &p, BoundingBox3F &aabb);
    
public:
    CollisionDetection();
    
    virtual ~CollisionDetection();
    
    void cleanup();
    
    float getTolerance() const {
        return m_tolerance;
    }
    
    void setTolerance(float val) {
        m_tolerance = val;
    }
    
    void addRigidBodyContact(const unsigned int rbIndex1, const unsigned int rbIndex2,
                             const Vector3F &cp1, const Vector3F &cp2,
                             const Vector3F &normal, const float dist,
                             const float restitutionCoeff, const float frictionCoeff);
    
    void addParticleRigidBodyContact(const unsigned int particleIndex, const unsigned int rbIndex,
                                     const Vector3F &cp1, const Vector3F &cp2,
                                     const Vector3F &normal, const float dist,
                                     const float restitutionCoeff, const float frictionCoeff);
    
    void addParticleSolidContact(const unsigned int particleIndex, const unsigned int solidIndex,
                                 const unsigned int tetIndex, const Vector3F &bary,
                                 const Vector3F &cp1, const Vector3F &cp2,
                                 const Vector3F &normal, const float dist,
                                 const float restitutionCoeff, const float frictionCoeff);
    
    virtual void addCollisionObject(const unsigned int bodyIndex, const unsigned int bodyType);
    
    std::vector<CollisionObject *> &getCollisionObjects() {
        return m_collisionObjects;
    }
    
    // virtual void collisionDetection(SimulationModel &model) = 0;
    
    void setContactCallback(CollisionDetection::ContactCallbackFunction val, void *userData);
    
    void setSolidContactCallback(CollisionDetection::SolidContactCallbackFunction val, void *userData);
    
    // void updateAABBs(SimulationModel &model);
    
    // void updateAABB(SimulationModel &model, CollisionDetection::CollisionObject *co);
};

}
}
#endif /* collision_detection_hpp */
