//
//  joint.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/3.
//

#ifndef joint_hpp
#define joint_hpp

#include "../physics.h"
#include "transform3.h"

namespace vox {
namespace physics {
/**
 * A base class providing common functionality for joints.
 */
class Joint {
public:
    void setActors(Collider *actor0, Collider *actor1);
    
    void setLocalPose(PxJointActorIndex::Enum actor, const Imath::Transform3 &localPose);
    
    Imath::Transform3 localPose(PxJointActorIndex::Enum actor) const;
    
    Imath::Transform3 relativeTransform() const;
    
    Imath::V3f relativeLinearVelocity() const;
    
    Imath::V3f relativeAngularVelocity() const;
    
    void setBreakForce(float force, float torque);
    
    void getBreakForce(float &force, float &torque) const;
    
    void setConstraintFlags(PxConstraintFlags flags);
    
    void setConstraintFlag(PxConstraintFlag::Enum flag, bool value);
    
    PxConstraintFlags constraintFlags() const;
    
    void setInvMassScale0(float invMassScale);
    
    float invMassScale0() const;
    
    void setInvInertiaScale0(float invInertiaScale);
    
    float invInertiaScale0() const;
    
    void setInvMassScale1(float invMassScale);
    
    float invMassScale1() const;
    
    void setInvInertiaScale1(float invInertiaScale);
    
    float invInertiaScale1() const;
    
protected:
    PxJoint *_nativeJoint;
};

}
}

#endif /* joint_hpp */
