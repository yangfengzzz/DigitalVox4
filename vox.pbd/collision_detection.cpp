//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "collision_detection.h"
#include "id_factory.h"

namespace vox {
namespace pbd {
int CollisionDetection::CollisionObjectWithoutGeometry::TYPE_ID = IDFactory::getId();
const unsigned int CollisionDetection::RigidBodyContactType = 0;
const unsigned int CollisionDetection::ParticleContactType = 1;
const unsigned int CollisionDetection::ParticleRigidBodyContactType = 2;
const unsigned int CollisionDetection::ParticleSolidContactType = 3;

const unsigned int CollisionDetection::CollisionObject::RigidBodyCollisionObjectType = 0;
const unsigned int CollisionDetection::CollisionObject::TriangleModelCollisionObjectType = 1;
const unsigned int CollisionDetection::CollisionObject::TetModelCollisionObjectType = 2;

CollisionDetection::CollisionDetection() : m_collisionObjects() {
    m_collisionObjects.reserve(1000);
    m_contactCB = NULL;
    m_solidContactCB = NULL;
    m_tolerance = 0.01f;
}

CollisionDetection::~CollisionDetection() {
    cleanup();
}

void CollisionDetection::cleanup() {
    for (unsigned int i = 0; i < m_collisionObjects.size(); i++)
        delete m_collisionObjects[i];
    m_collisionObjects.clear();
}

void CollisionDetection::addRigidBodyContact(const unsigned int rbIndex1, const unsigned int rbIndex2,
                                             const Vector3F &cp1, const Vector3F &cp2,
                                             const Vector3F &normal, const float dist,
                                             const float restitutionCoeff, const float frictionCoeff) {
    if (m_contactCB)
        m_contactCB(RigidBodyContactType, rbIndex1, rbIndex2, cp1, cp2, normal, dist, restitutionCoeff, frictionCoeff, m_contactCBUserData);
}

void CollisionDetection::addParticleRigidBodyContact(const unsigned int particleIndex, const unsigned int rbIndex,
                                                     const Vector3F &cp1, const Vector3F &cp2,
                                                     const Vector3F &normal, const float dist,
                                                     const float restitutionCoeff, const float frictionCoeff) {
    if (m_contactCB)
        m_contactCB(ParticleRigidBodyContactType, particleIndex, rbIndex, cp1, cp2, normal, dist, restitutionCoeff, frictionCoeff, m_contactCBUserData);
}

void CollisionDetection::addParticleSolidContact(const unsigned int particleIndex, const unsigned int solidIndex,
                                                 const unsigned int tetIndex, const Vector3F &bary, const Vector3F &cp1, const Vector3F &cp2,
                                                 const Vector3F &normal, const float dist, const float restitutionCoeff, const float frictionCoeff) {
    if (m_solidContactCB)
        m_solidContactCB(ParticleSolidContactType, particleIndex, solidIndex, tetIndex, bary, cp1, cp2, normal, dist, restitutionCoeff, frictionCoeff, m_contactCBUserData);
}

void CollisionDetection::addCollisionObject(const unsigned int bodyIndex, const unsigned int bodyType) {
    CollisionObjectWithoutGeometry *co = new CollisionObjectWithoutGeometry();
    co->m_bodyIndex = bodyIndex;
    co->m_bodyType = bodyType;
    m_collisionObjects.push_back(co);
}

void CollisionDetection::setContactCallback(CollisionDetection::ContactCallbackFunction val, void *userData) {
    m_contactCB = val;
    m_contactCBUserData = userData;
}

void CollisionDetection::setSolidContactCallback(CollisionDetection::SolidContactCallbackFunction val, void *userData) {
    m_solidContactCB = val;
    m_solidContactCBUserData = userData;
}

//void CollisionDetection::updateAABBs(SimulationModel &model) {
//    const SimulationModel::RigidBodyVector &rigidBodies = model.getRigidBodies();
//    const SimulationModel::TriangleModelVector &triModels = model.getTriangleModels();
//    const SimulationModel::TetModelVector &tetModels = model.getTetModels();
//    const ParticleData &pd = model.getParticles();
//
//    for (unsigned int i = 0; i < m_collisionObjects.size(); i++) {
//        CollisionDetection::CollisionObject *co = m_collisionObjects[i];
//        updateAABB(model, co);
//    }
//}
//
//void CollisionDetection::updateAABB(SimulationModel &model, CollisionDetection::CollisionObject *co) {
//    const SimulationModel::RigidBodyVector &rigidBodies = model.getRigidBodies();
//    const SimulationModel::TriangleModelVector &triModels = model.getTriangleModels();
//    const SimulationModel::TetModelVector &tetModels = model.getTetModels();
//    const ParticleData &pd = model.getParticles();
//    if (co->m_bodyType == CollisionDetection::CollisionObject::RigidBodyCollisionObjectType) {
//        const unsigned int rbIndex = co->m_bodyIndex;
//        RigidBody *rb = rigidBodies[rbIndex];
//        const VertexData &vd = rb->getGeometry().getVertexData();
//
//        co->m_aabb.m_p[0] = vd.getPosition(0);
//        co->m_aabb.m_p[1] = vd.getPosition(0);
//        for (unsigned int j = 1; j < vd.size(); j++) {
//            updateAABB(vd.getPosition(j), co->m_aabb);
//        }
//    } else if (co->m_bodyType == CollisionDetection::CollisionObject::TriangleModelCollisionObjectType) {
//        const unsigned int modelIndex = co->m_bodyIndex;
//        TriangleModel *tm = triModels[modelIndex];
//        const unsigned int offset = tm->getIndexOffset();
//        const IndexedFaceMesh &mesh = tm->getParticleMesh();
//        const unsigned int numVert = mesh.numVertices();
//
//        co->m_aabb.m_p[0] = pd.getPosition(offset);
//        co->m_aabb.m_p[1] = pd.getPosition(offset);
//        for (unsigned int j = offset + 1; j < offset + numVert; j++) {
//            updateAABB(pd.getPosition(j), co->m_aabb);
//        }
//    } else if (co->m_bodyType == CollisionDetection::CollisionObject::TetModelCollisionObjectType) {
//        const unsigned int modelIndex = co->m_bodyIndex;
//        TetModel *tm = tetModels[modelIndex];
//        const unsigned int offset = tm->getIndexOffset();
//        const IndexedTetMesh &mesh = tm->getParticleMesh();
//        const unsigned int numVert = mesh.numVertices();
//
//        co->m_aabb.m_p[0] = pd.getPosition(offset);
//        co->m_aabb.m_p[1] = pd.getPosition(offset);
//        for (unsigned int j = offset + 1; j < offset + numVert; j++) {
//            updateAABB(pd.getPosition(j), co->m_aabb);
//        }
//    }
//
//    // Extend BoundingBox3Fby tolerance
//    co->m_aabb.m_p[0][0] -= m_tolerance;
//    co->m_aabb.m_p[0][1] -= m_tolerance;
//    co->m_aabb.m_p[0][2] -= m_tolerance;
//    co->m_aabb.m_p[1][0] += m_tolerance;
//    co->m_aabb.m_p[1][1] += m_tolerance;
//    co->m_aabb.m_p[1][2] += m_tolerance;
//}

void CollisionDetection::updateAABB(const Vector3F &p, BoundingBox3F&aabb) {
    if (aabb.lowerCorner.x > p.x)
        aabb.lowerCorner.x = p.x;
    if (aabb.lowerCorner.y > p.y)
        aabb.lowerCorner.y = p.y;
    if (aabb.lowerCorner.z > p.z)
        aabb.lowerCorner.z = p.z;
    if (aabb.upperCorner.x < p.x)
        aabb.upperCorner.x = p.x;
    if (aabb.upperCorner.y < p.y)
        aabb.upperCorner.y = p.y;
    if (aabb.upperCorner.z < p.z)
        aabb.upperCorner.z = p.z;
}


}
}
