//
//  free_control.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/17.
//

#ifndef free_control_hpp
#define free_control_hpp

#include "../script.h"
#include "spherical.h"
#include "ImathVec.h"
#include <array>

namespace vox {
namespace control {
/**
 * The camera's roaming controller, can move up and down, left and right, and rotate the viewing angle.
 */
class FreeControl : public Script {
public:
    FreeControl(Entity *entity);
    
    void onDisable() override;
    
    void onEnable() override;
    
    void onDestroy() override;
    
    void onUpdate(float delta) override;
    
    void inputEvent(const InputEvent &input_event) override;
    
    void resize(uint32_t width, uint32_t height) override;

public:
    /**
     * Keyboard press event.
     */
    void onKeyDown(KeyCode key);
    
    /**
     * Keyboard up event.
     */
    void onKeyUp(KeyCode key);
    
public:
    /**
     * Mouse press event.
     */
    void onMouseDown(double xpos, double ypos);
    
    /**
     * Mouse up event.
     */
    void onMouseUp();
    
    /**
     * Mouse movement event.
     */
    void onMouseMove(double xpos, double ypos);
    
    /**
     * The angle of rotation around the y axis and the x axis respectively.
     * @param alpha - Radian to rotate around the y axis
     * @param beta - Radian to rotate around the x axis
     */
    void rotate(float alpha = 0, float beta = 0);
        
    /**
     * must updateSpherical after quaternion has been changed
     * @example
     * Entity#lookAt([0,1,0],[0,1,0]);
     * AFreeControls#updateSpherical();
     */
    void updateSpherical();
    
private:
    Imath::V3f _forward;
    Imath::V3f _right;
    
    /**
     * Movement distance per second, the unit is the unit before MVP conversion.
     */
    float movementSpeed = 1.0;
    
    /**
     * Rotate speed.
     */
    float rotateSpeed = 1.0;
    
    /**
     * Simulate a ground.
     */
    bool floorMock = false;
    
    /**
     * Simulated ground height.
     */
    float floorY = 0;
    
    /**
     * Only rotate when press=true
     */
    bool press = false;
    
    /**
     * Radian of spherical.theta.
     */
    float _theta = 0;
    
    /**
     * Radian of spherical.phi.
     */
    float _phi = 0;
    
    bool _moveForward = false;
    bool _moveBackward = false;
    bool _moveLeft = false;
    bool _moveRight = false;
    
    Imath::V3f _v3Cache;
    Spherical _spherical;
    std::array<double, 2> _rotateOri{};
    
    bool _enableEvent = true;
    uint32_t _width = 1000;
    uint32_t _height = 1000;
};

}
}

#endif /* free_control_hpp */
