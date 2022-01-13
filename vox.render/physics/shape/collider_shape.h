//
//  collider_shape.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/2.
//

#ifndef collider_shape_hpp
#define collider_shape_hpp

#include "../physics.h"
#include "transform3.h"
#include "ImathEuler.h"
#include <vector>

namespace vox {
namespace physics {
class ColliderShape {
public:
    ColliderShape();
    
    Collider *collider();
    
public:
    void setLocalPose(const Imath::Transform3 &pose);
    
    Imath::Transform3 localPose() const;
    
    void setPosition(const Imath::V3f &pos);
    
    Imath::V3f position() const;
    
    virtual void setWorldScale(const Imath::V3f &scale) = 0;
    
public:
    void setMaterial(PxMaterial *materials);
    
    PxMaterial *material();
    
public:
    void setQueryFilterData(const PxFilterData &data);
    
    PxFilterData queryFilterData();
    
    uint32_t uniqueID();
    
public:
    void setFlag(PxShapeFlag::Enum flag, bool value);
    
    void setFlags(PxShapeFlags inFlags);
    
    PxShapeFlags getFlags() const;
    
    bool trigger();
    
    void setTrigger(bool isTrigger);
    
    bool sceneQuery();
    
    void setSceneQuery(bool isQuery);
    
protected:
    friend class Collider;
    
    PxShape *_nativeShape = nullptr;
    std::shared_ptr<PxGeometry> _nativeGeometry = nullptr;
    PxMaterial *_nativeMaterial = nullptr;
    
    Collider *_collider = nullptr;
    
    Imath::V3f _scale = Imath::V3f(1, 1, 1);
    Imath::Transform3 _pose;
    static constexpr float halfSqrt = 0.70710678118655;
};

}
}
#endif /* collider_shape_hpp */
