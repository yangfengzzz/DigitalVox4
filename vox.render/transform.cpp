//
//  transform.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/12.
//

#include "transform.h"
#include "math_utils.h"
#include "entity.h"
#include "ImathMatrixAlgo.h"
#include "ImathRightHand.h"

namespace vox {
Transform::Transform(Entity *entity) : Component(entity) {
}

V3f Transform::position() {
    return _position;
}

void Transform::setPosition(const V3f &value) {
    _position = value;
    _setDirtyFlagTrue(TransformFlag::LocalMatrix);
    _updateWorldPositionFlag();
}

V3f Transform::worldPosition() {
    if (_isContainDirtyFlag(TransformFlag::WorldPosition)) {
        if (_getParentTransform()) {
            _worldPosition = worldMatrix().translation();
        } else {
            _worldPosition = _position;
        }
        _setDirtyFlagFalse(TransformFlag::WorldPosition);
    }
    return _worldPosition;
}

void Transform::setWorldPosition(const V3f &value) {
    _worldPosition = value;
    const auto parent = _getParentTransform();
    if (parent) {
        const auto _tempMat41 = parent->worldMatrix().inverse();
        _tempMat41.multVecMatrix(value, _position);
    } else {
        _position = value;
    }
    setPosition(_position);
    _setDirtyFlagFalse(TransformFlag::WorldPosition);
}

V3f Transform::rotation() {
    if (_isContainDirtyFlag(TransformFlag::LocalEuler)) {
        _rotation.extract(_rotationQuaternion);
        _rotation *= kRadianToDegree; // radians to degrees
        
        _setDirtyFlagFalse(TransformFlag::LocalEuler);
    }
    return _rotation;
}

void Transform::setRotation(const V3f &value) {
    _rotation = value;
    _setDirtyFlagTrue(TransformFlag::LocalMatrix | TransformFlag::LocalQuat);
    _setDirtyFlagFalse(TransformFlag::LocalEuler);
    _updateWorldRotationFlag();
}

V3f Transform::worldRotation() {
    if (_isContainDirtyFlag(TransformFlag::WorldEuler)) {
        _worldRotation.extract(worldRotationQuaternion());
        _worldRotation *= kRadianToDegree; // Radian to angle
        _setDirtyFlagFalse(TransformFlag::WorldEuler);
    }
    return _worldRotation;
}

void Transform::setWorldRotation(const V3f &value) {
    _worldRotation = value;
    _worldRotationQuaternion = Eulerf(degreesToRadians(value.y), degreesToRadians(value.x), degreesToRadians(value.z)).toQuat();
    setWorldRotationQuaternion(_worldRotationQuaternion);
    _setDirtyFlagFalse(TransformFlag::WorldEuler);
}

Quatf Transform::rotationQuaternion() {
    if (_isContainDirtyFlag(TransformFlag::LocalQuat)) {
        _rotationQuaternion = Eulerf(degreesToRadians(_rotation.y), degreesToRadians(_rotation.x), degreesToRadians(_rotation.z)).toQuat();
        _setDirtyFlagFalse(TransformFlag::LocalQuat);
    }
    return _rotationQuaternion;
}

void Transform::setRotationQuaternion(const Quatf &value) {
    _rotationQuaternion = value;
    _setDirtyFlagTrue(TransformFlag::LocalMatrix | TransformFlag::LocalEuler);
    _setDirtyFlagFalse(TransformFlag::LocalQuat);
    _updateWorldRotationFlag();
}

Quatf Transform::worldRotationQuaternion() {
    if (_isContainDirtyFlag(TransformFlag::WorldQuat)) {
        const auto parent = _getParentTransform();
        if (parent) {
            _worldRotationQuaternion = parent->worldRotationQuaternion() * rotationQuaternion();
        } else {
            _worldRotationQuaternion = rotationQuaternion();
        }
        _setDirtyFlagFalse(TransformFlag::WorldQuat);
    }
    return _worldRotationQuaternion;
}

void Transform::setWorldRotationQuaternion(const Quatf &value) {
    _worldRotationQuaternion = value;
    const auto parent = _getParentTransform();
    if (parent) {
        auto _tempQuat0 = parent->worldRotationQuaternion().inverse();
        _rotationQuaternion = value * _tempQuat0;
    } else {
        _rotationQuaternion = value;
    }
    setRotationQuaternion(_rotationQuaternion);
    _setDirtyFlagFalse(TransformFlag::WorldQuat);
}

V3f Transform::scale() {
    return _scale;
}

void Transform::setScale(const V3f &value) {
    _scale = value;
    _setDirtyFlagTrue(TransformFlag::LocalMatrix);
    _updateWorldScaleFlag();
}

V3f Transform::lossyWorldScale() {
    if (_isContainDirtyFlag(TransformFlag::WorldScale)) {
        if (_getParentTransform()) {
            extractScaling(worldMatrix(), _lossyWorldScale);
        } else {
            _lossyWorldScale = _scale;
        }
        _setDirtyFlagFalse(TransformFlag::WorldScale);
    }
    return _lossyWorldScale;
}

M44f Transform::localMatrix() {
    if (_isContainDirtyFlag(TransformFlag::LocalMatrix)) {
        _localMatrix = rotationQuaternion().toMatrix44().scale(_scale);
        _localMatrix = _localMatrix.translate(_position);
        _setDirtyFlagFalse(TransformFlag::LocalMatrix);
    }
    return _localMatrix;
}

void Transform::setLocalMatrix(const M44f &value) {
    _localMatrix = value;
    V3f dump;
    extractSHRT(_localMatrix, _scale, dump, _rotation, _position);
    _rotationQuaternion = _rotation.toQuat();
    _setDirtyFlagTrue(TransformFlag::LocalEuler);
    _setDirtyFlagFalse(TransformFlag::LocalMatrix);
    _updateAllWorldFlag();
}

M44f Transform::worldMatrix() {
    if (_isContainDirtyFlag(TransformFlag::WorldMatrix)) {
        const auto parent = _getParentTransform();
        if (parent) {
            _worldMatrix = parent->worldMatrix() * localMatrix();
        } else {
            _worldMatrix = localMatrix();
        }
        _setDirtyFlagFalse(TransformFlag::WorldMatrix);
    }
    return _worldMatrix;
}

void Transform::setWorldMatrix(const M44f &value) {
    _worldMatrix = value;
    const auto parent = _getParentTransform();
    if (parent) {
        auto _tempMat42 = parent->worldMatrix().inverse();
        _localMatrix = value * _tempMat42;
    } else {
        _localMatrix = value;
    }
    setLocalMatrix(_localMatrix);
    _setDirtyFlagFalse(TransformFlag::WorldMatrix);
}

void Transform::setPosition(float x, float y, float z) {
    _position = V3f(x, y, z);
    setPosition(_position);
}

void Transform::setRotation(float x, float y, float z) {
    _rotation = V3f(x, y, z);
    setRotation(_rotation);
}

void Transform::setRotationQuaternion(float x, float y, float z, float w) {
    _rotationQuaternion = Quatf(w, x, y, z);
    setRotationQuaternion(_rotationQuaternion);
}

void Transform::setScale(float x, float y, float z) {
    _scale = V3f(x, y, z);
    setScale(_scale);
}

void Transform::setWorldPosition(float x, float y, float z) {
    _worldPosition = V3f(x, y, z);
    setWorldPosition(_worldPosition);
}

void Transform::setWorldRotation(float x, float y, float z) {
    _worldRotation = V3f(x, y, z);
    setWorldRotation(_worldRotation);
}

void Transform::setWorldRotationQuaternion(float x, float y, float z, float w) {
    _worldRotationQuaternion = Quatf(w, x, y, z);
    setWorldRotationQuaternion(_worldRotationQuaternion);
}

V3f Transform::worldForward() {
    const auto &e = worldMatrix().getValue();
    auto forward = V3f(-e[8], -e[9], -e[10]);
    return forward.normalize();
}

V3f Transform::worldRight() {
    const auto &e = worldMatrix().getValue();
    auto right = V3f(e[0], e[1], e[2]);
    return right.normalize();
}

V3f Transform::worldUp() {
    const auto &e = worldMatrix().getValue();
    auto up = V3f(e[4], e[5], e[6]);
    return up.normalize();
}

void Transform::translate(const V3f &translation, bool relativeToLocal) {
    _translate(translation, relativeToLocal);
}

void Transform::translate(float x, float y, float z, bool relativeToLocal) {
    V3f translate = V3f(x, y, z);
    _translate(translate, relativeToLocal);
}

void Transform::rotate(const V3f &rotation, bool relativeToLocal) {
    _rotateXYZ(rotation.x, rotation.y, rotation.z, relativeToLocal);
}

void Transform::rotate(float x, float y, float z, bool relativeToLocal) {
    _rotateXYZ(x, y, z, relativeToLocal);
}

void Transform::rotateByAxis(const V3f &axis, float angle, bool relativeToLocal) {
    const auto rad = angle * kDegreeToRadian;
    Quatf _tempQuat0;
    _tempQuat0.setAxisAngle(axis, rad);
    _rotateByQuat(_tempQuat0, relativeToLocal);
}

void Transform::lookAt(const V3f &worldPosition, const V3f &worldUp) {
    const auto position = this->worldPosition();
    const auto EPSILON = 1e-6f;;
    if (std::abs(position.x - worldPosition.x) < EPSILON &&
        std::abs(position.y - worldPosition.y) < EPSILON &&
        std::abs(position.z - worldPosition.z) < EPSILON) {
        return;
    }
    M44f rotMat = Imath::lookAt(position, worldPosition, worldUp);
    Eulerf worldRotation;
    Imath::extractEulerXYZ(rotMat, worldRotation);
    setWorldRotationQuaternion(worldRotation.toQuat().inverse());
}

std::unique_ptr<UpdateFlag> Transform::registerWorldChangeFlag() {
    return _updateFlagManager.registration();
}

void Transform::_parentChange() {
    _isParentDirty = true;
    _updateAllWorldFlag();
}

void Transform::_updateWorldPositionFlag() {
    if (!_isContainDirtyFlags(TransformFlag::WmWp)) {
        _worldAssociatedChange(TransformFlag::WmWp);
        const auto &nodeChildren = _entity->_children;
        for (size_t i = 0, n = nodeChildren.size(); i < n; i++) {
            nodeChildren[i]->transform->_updateWorldPositionFlag();
        }
    }
}

void Transform::_updateWorldRotationFlag() {
    if (!_isContainDirtyFlags(TransformFlag::WmWeWq)) {
        _worldAssociatedChange(TransformFlag::WmWeWq);
        const auto &nodeChildren = _entity->_children;
        for (size_t i = 0, n = nodeChildren.size(); i < n; i++) {
            // Rotation update of parent entity will trigger world position and rotation update of all child entity.
            nodeChildren[i]->transform->_updateWorldPositionAndRotationFlag();
        }
    }
}

void Transform::_updateWorldPositionAndRotationFlag() {
    if (!_isContainDirtyFlags(TransformFlag::WmWpWeWq)) {
        _worldAssociatedChange(TransformFlag::WmWpWeWq);
        const auto &nodeChildren = _entity->_children;
        for (size_t i = 0, n = nodeChildren.size(); i < n; i++) {
            nodeChildren[i]->transform->_updateWorldPositionAndRotationFlag();
        }
    }
}

void Transform::_updateWorldScaleFlag() {
    if (!_isContainDirtyFlags(TransformFlag::WmWs)) {
        _worldAssociatedChange(TransformFlag::WmWs);
        const auto &nodeChildren = _entity->_children;
        for (size_t i = 0, n = nodeChildren.size(); i < n; i++) {
            nodeChildren[i]->transform->_updateWorldPositionAndScaleFlag();
        }
    }
}

void Transform::_updateWorldPositionAndScaleFlag() {
    if (!_isContainDirtyFlags(TransformFlag::WmWpWs)) {
        _worldAssociatedChange(TransformFlag::WmWpWs);
        const auto &nodeChildren = _entity->_children;
        for (size_t i = 0, n = nodeChildren.size(); i < n; i++) {
            nodeChildren[i]->transform->_updateWorldPositionAndScaleFlag();
        }
    }
}

void Transform::_updateAllWorldFlag() {
    if (!_isContainDirtyFlags(TransformFlag::WmWpWeWqWs)) {
        _worldAssociatedChange(TransformFlag::WmWpWeWqWs);
        const auto &nodeChildren = _entity->_children;
        for (size_t i = 0, n = nodeChildren.size(); i < n; i++) {
            nodeChildren[i]->transform->_updateAllWorldFlag();
        }
    }
}

Transform *Transform::_getParentTransform() {
    if (!_isParentDirty) {
        return _parentTransformCache;
    }
    Transform *parentCache = nullptr;
    auto parent = _entity->parent();
    while (parent) {
        const auto &transform = parent->transform;
        if (transform) {
            parentCache = transform;
            break;
        } else {
            parent = parent->parent();
        }
    }
    _parentTransformCache = parentCache;
    _isParentDirty = false;
    return parentCache;
}

bool Transform::_isContainDirtyFlags(int targetDirtyFlags) {
    return (_dirtyFlag & targetDirtyFlags) == targetDirtyFlags;
}

bool Transform::_isContainDirtyFlag(int type) {
    return (_dirtyFlag & type) != 0;
}

void Transform::_setDirtyFlagTrue(int type) {
    _dirtyFlag |= type;
}

void Transform::_setDirtyFlagFalse(int type) {
    _dirtyFlag &= ~type;
}

void Transform::_worldAssociatedChange(int type) {
    _dirtyFlag |= type;
    _updateFlagManager.distribute();
}

void Transform::_rotateByQuat(const Quatf &rotateQuat, bool relativeToLocal) {
    if (relativeToLocal) {
        _rotationQuaternion = rotationQuaternion() * rotateQuat;
        setRotationQuaternion(_rotationQuaternion);
    } else {
        _worldRotationQuaternion = worldRotationQuaternion() * rotateQuat;
        setWorldRotationQuaternion(_worldRotationQuaternion);
    }
}

void Transform::_translate(const V3f &translation, bool relativeToLocal) {
    if (relativeToLocal) {
        _position = _position + translation;
        setPosition(_position);
    } else {
        _worldPosition = _worldPosition + translation;
        setWorldPosition(_worldPosition);
    }
}

void Transform::_rotateXYZ(float x, float y, float z, bool relativeToLocal) {
    const auto rotQuat = Eulerf(y * kDegreeToRadian, x * kDegreeToRadian, z * kDegreeToRadian).toQuat();
    _rotateByQuat(rotQuat, relativeToLocal);
}

}
