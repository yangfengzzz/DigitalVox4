//
//  light.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/19.
//

#ifndef light_hpp
#define light_hpp

#include "component.h"
#include "matrix4x4.h"

namespace vox {
/**
 * Light base class.
 */
class Light : public Component {
public:
    /**
     * Each type of light source is at most 10, beyond which it will not take effect.
     * */
    static constexpr uint32_t MAX_LIGHT = 10;
    
    Light(Entity *entity);
    
    /**
     * View matrix.
     */
    Matrix4x4F viewMatrix();
    
    /**
     * Inverse view matrix.
     */
    Matrix4x4F inverseViewMatrix();
    
public:
    bool enableShadow() {
        return _enableShadow;
    }
    
    void setEnableShadow(bool enabled) {
        _enableShadow = enabled;
    }
    
    virtual Matrix4x4F shadowProjectionMatrix() = 0;
    
private:
    bool _enableShadow = false;
};

}
#endif /* light_hpp */
