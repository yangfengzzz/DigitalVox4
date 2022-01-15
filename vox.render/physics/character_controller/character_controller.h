//
//  CharacterController.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/3.
//

#ifndef CharacterController_hpp
#define CharacterController_hpp

#include "../physics.h"
#include "../../component.h"

namespace vox {
namespace physics {

class CharacterController : public Component {
public:
    CharacterController(Entity *entity);
    
    PxControllerCollisionFlags move(const Imath::V3f &disp, float minDist, float elapsedTime);
    
    bool setPosition(const Imath::V3f &position);
    
    Imath::V3f position() const;
    
    bool setFootPosition(const Imath::V3f &position);
    
    Imath::V3f footPosition() const;
    
    void setStepOffset(const float offset);
    
    float stepOffset() const;
    
    void setNonWalkableMode(PxControllerNonWalkableMode::Enum flag);
    
    PxControllerNonWalkableMode::Enum nonWalkableMode() const;
    
    float contactOffset() const;
    
    void setContactOffset(float offset);
    
    Imath::V3f upDirection() const;
    
    void setUpDirection(const Imath::V3f &up);
    
    float slopeLimit() const;
    
    void setSlopeLimit(float slopeLimit);
    
    void invalidateCache();
    
    void state(PxControllerState &state) const;
    
    void stats(PxControllerStats &stats) const;
    
    void resize(float height);
    
private:
    friend class PhysicsManager;
    
    void _onLateUpdate();
    
    void _onEnable() override;
    
    void _onDisable() override;
    
protected:
    PxController *_nativeController;
};

}
}

#endif /* CharacterController_hpp */
