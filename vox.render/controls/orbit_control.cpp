//
//  orbit_control.cpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/30.
//

#include "orbit_control.h"
#include "../entity.h"

namespace vox {
namespace control {
OrbitControl::OrbitControl(Entity *entity) :
Script(entity),
camera(entity) {
}

void OrbitControl::onDisable() {
    _enableEvent = false;
}

void OrbitControl::onEnable() {
    _enableEvent = true;
}

void OrbitControl::onDestroy() {
    onDisable();
}

void OrbitControl::resize(uint32_t width, uint32_t height) {
    _width = width;
    _height = height;
}

void OrbitControl::inputEvent(const InputEvent &input_event) {
    if (_enableEvent) {
        if (input_event.get_source() == EventSource::Keyboard) {
            const auto &key_event = static_cast<const KeyInputEvent &>(input_event);
            onKeyDown(key_event.get_code());
        } else if (input_event.get_source() == EventSource::Mouse) {
            const auto &mouse_button = static_cast<const MouseButtonInputEvent &>(input_event);
            if (mouse_button.get_action() == MouseAction::Down) {
                onMouseDown(mouse_button.get_button(), mouse_button.get_pos_x(), mouse_button.get_pos_y());
                _enableMove = true;
            } else {
                onMouseUp();
                _enableMove = false;
            }
            
            if (_enableMove && mouse_button.get_action() == MouseAction::Move) {
                onMouseMove(mouse_button.get_pos_x(), mouse_button.get_pos_y());
            }
        } else if (input_event.get_source() == EventSource::Scroll) {
            const auto &scroll_event = static_cast<const ScrollInputEvent &>(input_event);
            onMouseWheel(scroll_event.get_offset_x(), scroll_event.get_offset_y());
        } else if (input_event.get_source() == EventSource::Touchscreen) {
            // TODO
        }
    }
}

void OrbitControl::onUpdate(float dtime) {
    if (!enabled()) return;
    
    const auto &position = camera->transform->position();
    _offset = position;
    _offset = _offset - target;
    _spherical.setFromVec3(_offset);
    
    if (autoRotate && _state == STATE::NONE) {
        rotateLeft(autoRotationAngle(dtime));
    }
    
    _spherical.theta += _sphericalDelta.theta;
    _spherical.phi += _sphericalDelta.phi;
    
    _spherical.theta = std::max(minAzimuthAngle, std::min(maxAzimuthAngle, _spherical.theta));
    _spherical.phi = std::max(minPolarAngle, std::min(maxPolarAngle, _spherical.phi));
    _spherical.makeSafe();
    
    if (_scale != 1) {
        _zoomFrag = _spherical.radius * (_scale - 1);
    }
    
    _spherical.radius += _zoomFrag;
    _spherical.radius = std::max(minDistance, std::min(maxDistance, _spherical.radius));
    
    target = target + _panOffset;
    _spherical.setToVec3(_offset);
    _position = target;
    _position = _position + _offset;
    
    camera->transform->setPosition(_position);
    camera->transform->lookAt(target, up);
    
    if (enableDamping == true) {
        _sphericalDump.theta *= 1 - dampingFactor;
        _sphericalDump.phi *= 1 - dampingFactor;
        _zoomFrag *= 1 - zoomFactor;
        
        if (_isMouseUp) {
            _sphericalDelta.theta = _sphericalDump.theta;
            _sphericalDelta.phi = _sphericalDump.phi;
        } else {
            _sphericalDelta.set(0, 0, 0);
        }
    } else {
        _sphericalDelta.set(0, 0, 0);
        _zoomFrag = 0;
    }
    
    _scale = 1;
    _panOffset = Imath::V3f(0, 0, 0);
}

float OrbitControl::autoRotationAngle(float dtime) {
    return (autoRotateSpeed / 1000) * dtime;
}

float OrbitControl::zoomScale() {
    return std::pow(0.95, zoomSpeed);
}

void OrbitControl::rotateLeft(float radian) {
    _sphericalDelta.theta -= radian;
    if (enableDamping) {
        _sphericalDump.theta = -radian;
    }
}

void OrbitControl::rotateUp(float radian) {
    _sphericalDelta.phi -= radian;
    if (enableDamping) {
        _sphericalDump.phi = -radian;
    }
}

void OrbitControl::panLeft(float distance, const Imath::M44f &worldMatrix) {
    const auto &e = worldMatrix.getValue();
    _vPan = Imath::V3f(e[0], e[1], e[2]);
    _vPan = _vPan * distance;
    _panOffset = _panOffset + _vPan;
}

void OrbitControl::panUp(float distance, const Imath::M44f &worldMatrix) {
    const auto &e = worldMatrix.getValue();
    _vPan = Imath::V3f(e[4], e[5], e[6]);
    _vPan = _vPan * distance;
    _panOffset = _panOffset + _vPan;
}

void OrbitControl::pan(float deltaX, float deltaY) {
    // perspective only
    Imath::V3f position = camera->transform->position();
    _vPan = position;
    _vPan = _vPan - target;
    auto targetDistance = _vPan.length();
    
    targetDistance *= (fov / 2) * (M_PI / 180);
    
    panLeft(-2 * deltaX * (targetDistance / float(_width)), camera->transform->worldMatrix());
    panUp(2 * deltaY * (targetDistance / float(_height)), camera->transform->worldMatrix());
}

void OrbitControl::zoomIn(float zoomScale) {
    _scale *= zoomScale;
}

void OrbitControl::zoomOut(float zoomScale) {
    _scale /= zoomScale;
}

//MARK: - Mouse
void OrbitControl::handleMouseDownRotate(double xpos, double ypos) {
    _rotateStart = Imath::V2f(xpos, ypos);
}

void OrbitControl::handleMouseDownZoom(double xpos, double ypos) {
    _zoomStart = Imath::V2f(xpos, ypos);
}

void OrbitControl::handleMouseDownPan(double xpos, double ypos) {
    _panStart = Imath::V2f(xpos, ypos);
}

void OrbitControl::handleMouseMoveRotate(double xpos, double ypos) {
    _rotateEnd = Imath::V2f(xpos, ypos);
    _rotateDelta = _rotateEnd - _rotateStart;
    
    rotateLeft(2 * M_PI * (_rotateDelta.x / float(_width)) * rotateSpeed);
    rotateUp(2 * M_PI * (_rotateDelta.y / float(_height)) * rotateSpeed);
    
    _rotateStart = _rotateEnd;
}

void OrbitControl::handleMouseMoveZoom(double xpos, double ypos) {
    _zoomEnd = Imath::V2f(xpos, ypos);
    _zoomDelta = _zoomEnd - _zoomStart;
    
    if (_zoomDelta.y > 0) {
        zoomOut(zoomScale());
    } else if (_zoomDelta.y < 0) {
        zoomIn(zoomScale());
    }
    
    _zoomStart = _zoomEnd;
}

void OrbitControl::handleMouseMovePan(double xpos, double ypos) {
    _panEnd = Imath::V2f(xpos, ypos);
    _panDelta = _panEnd - _panStart;
    
    pan(_panDelta.x, _panDelta.y);
    
    _panStart = _panEnd;
}

void OrbitControl::handleMouseWheel(double xoffset, double yoffset) {
    if (yoffset < 0) {
        zoomIn(zoomScale());
    } else if (yoffset > 0) {
        zoomOut(zoomScale());
    }
}

void OrbitControl::onMouseDown(MouseButton button, double xpos, double ypos) {
    if (enabled() == false) return;
    
    _isMouseUp = false;
    
    switch (button) {
        case MouseButton::Left:
            if (enableRotate == false) return;
            
            handleMouseDownRotate(xpos, ypos);
            _state = STATE::ROTATE;
            break;
        case MouseButton::Middle:
            if (enableZoom == false) return;
            
            handleMouseDownZoom(xpos, ypos);
            _state = STATE::ZOOM;
            break;
        case MouseButton::Right:
            if (enablePan == false) return;
            
            handleMouseDownPan(xpos, ypos);
            _state = STATE::PAN;
            break;
        default:
            break;
    }
}

void OrbitControl::onMouseMove(double xpos, double ypos) {
    if (enabled() == false) return;
    
    switch (_state) {
        case STATE::ROTATE:
            if (enableRotate == false) return;
            
            handleMouseMoveRotate(xpos, ypos);
            break;
            
        case STATE::ZOOM:
            if (enableZoom == false) return;
            
            handleMouseMoveZoom(xpos, ypos);
            break;
            
        case STATE::PAN:
            if (enablePan == false) return;
            
            handleMouseMovePan(xpos, ypos);
            break;
        default:
            break;;
    }
}

void OrbitControl::onMouseUp() {
    if (enabled() == false) return;
    
    _isMouseUp = true;
    _state = STATE::NONE;
}

void OrbitControl::onMouseWheel(double xoffset, double yoffset) {
    if (enabled() == false || enableZoom == false ||
        (_state != STATE::NONE && _state != STATE::ROTATE))
        return;
    
    handleMouseWheel(xoffset, yoffset);
}

//MARK: - KeyBoard
void OrbitControl::handleKeyDown(KeyCode key) {
    switch (key) {
        case KeyCode::Up:
            pan(0, keyPanSpeed);
            break;
        case KeyCode::Down:
            pan(0, -keyPanSpeed);
            break;
        case KeyCode::Left:
            pan(keyPanSpeed, 0);
            break;
        case KeyCode::Right:
            pan(-keyPanSpeed, 0);
            break;
        default:
            break;
    }
}

void OrbitControl::onKeyDown(KeyCode key) {
    if (enabled() == false || enableKeys == false || enablePan == false) return;
    
    handleKeyDown(key);
}

//MARK: - Touch
void OrbitControl::handleTouchStartRotate() {
}

void OrbitControl::handleTouchStartZoom() {
}

void OrbitControl::handleTouchStartPan() {
}

void OrbitControl::handleTouchMoveRotate() {
}

void OrbitControl::handleTouchMoveZoom() {
}

void OrbitControl::handleTouchMovePan() {
}

void OrbitControl::onTouchStart() {
}

void OrbitControl::onTouchMove() {
}

void OrbitControl::onTouchEnd() {
}

}
}
