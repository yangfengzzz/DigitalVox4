//
//  physx_dynamic_app.hpp
//  apps
//
//  Created by 杨丰 on 2022/1/20.
//

#ifndef physx_dynamic_app_hpp
#define physx_dynamic_app_hpp

#include "forward_application.h"
#include <random>

namespace vox {
class PhysXDynamicApp : public ForwardApplication {
public:
    void loadScene(uint32_t width, uint32_t height) override;
    
    void inputEvent(const InputEvent &inputEvent) override;
    
private:
    EntityPtr addSphere(float radius, const Point3F &position,
                        const QuaternionF &rotation, const Vector3F &velocity);
    
    EntityPtr addCapsule(float radius, float height,
                         const Point3F &position, const QuaternionF &rotation);
    
private:
    EntityPtr _rootEntity{nullptr};
    std::default_random_engine e;
    std::uniform_real_distribution<float> u;
};

}

#endif /* physx_dynamic_app_hpp */
