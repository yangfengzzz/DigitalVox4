//
//  camera.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/12.
//

#include "camera.h"
#include "entity.h"
#include "scene.h"
#include "ImathRightHand.h"
#include "shader/shader.h"

namespace vox {
namespace sg {
Camera::Camera(Entity *entity) :
Component(entity),
_viewMatrixProperty(Shader::createProperty("u_viewMat", ShaderDataGroup::Camera)),
_projectionMatrixProperty(Shader::createProperty("u_projMat", ShaderDataGroup::Camera)),
_vpMatrixProperty(Shader::createProperty("u_VPMat", ShaderDataGroup::Camera)),
_inverseViewMatrixProperty(Shader::createProperty("u_viewInvMat", ShaderDataGroup::Camera)),
_inverseProjectionMatrixProperty(Shader::createProperty("u_projInvMat", ShaderDataGroup::Camera)),
_cameraPositionProperty(Shader::createProperty("u_cameraPos", ShaderDataGroup::Camera)) {
    auto transform = entity->transform;
    _transform = transform;
    _isViewMatrixDirty = transform->registerWorldChangeFlag();
    _isInvViewProjDirty = transform->registerWorldChangeFlag();
    _frustumViewChangeFlag = transform->registerWorldChangeFlag();
}

float Camera::nearClipPlane() const {
    return _nearClipPlane;
}

void Camera::setNearClipPlane(float value) {
    _nearClipPlane = value;
    _projMatChange();
}

float Camera::farClipPlane() const {
    return _farClipPlane;
}

void Camera::setFarClipPlane(float value) {
    _farClipPlane = value;
    _projMatChange();
}

float Camera::fieldOfView() const {
    return _fieldOfView;
}

void Camera::setFieldOfView(float value) {
    _fieldOfView = value;
    _projMatChange();
}

float Camera::aspectRatio() const {
    if (_customAspectRatio == std::nullopt) {
        return (_width * _viewport.z) / (_height * _viewport.w);
    } else {
        return _customAspectRatio.value();
    }
}

void Camera::setAspectRatio(float value) {
    _customAspectRatio = value;
    _projMatChange();
}

Imath::V4f Camera::viewport() const {
    return _viewport;
}

void Camera::setViewport(const Imath::V4f &value) {
    _viewport = value;
    _projMatChange();
}

bool Camera::isOrthographic() const {
    return _isOrthographic;
}

void Camera::setIsOrthographic(bool value) {
    _isOrthographic = value;
    _projMatChange();
}

float Camera::orthographicSize() const {
    return _orthographicSize;
}

void Camera::setOrthographicSize(float value) {
    _orthographicSize = value;
    _projMatChange();
}

Imath::M44f Camera::viewMatrix() {
    // Remove scale
    if (_isViewMatrixDirty->flag) {
        _isViewMatrixDirty->flag = false;
        _viewMatrix = _transform->worldMatrix().inverse();
    }
    return _viewMatrix;
}

void Camera::setProjectionMatrix(const Imath::M44f &value) {
    _projectionMatrix = value;
    _isProjMatSetting = true;
    _projMatChange();
}

Imath::M44f Camera::projectionMatrix() {
    if ((!_isProjectionDirty || _isProjMatSetting) &&
        _lastAspectSize.x == _width &&
        _lastAspectSize.y == _height) {
        return _projectionMatrix;
    }
    _isProjectionDirty = false;
    _lastAspectSize.x = _width;
    _lastAspectSize.y = _height;
    if (!_isOrthographic) {
        _projectionMatrix = Imath::perspective(degreesToRadians(_fieldOfView),
                                                aspectRatio(),
                                                _nearClipPlane,
                                                _farClipPlane);
    } else {
        const auto width = _orthographicSize * aspectRatio();
        const auto height = _orthographicSize;
        _projectionMatrix = Imath::ortho(-width, width, -height, height, _nearClipPlane, _farClipPlane);
    }
    return _projectionMatrix;
}

bool Camera::enableHDR() {
    return false;
}

void Camera::setEnableHDR(bool value) {
    assert(false && "not implementation");
}

void Camera::resetProjectionMatrix() {
    _isProjMatSetting = false;
    _projMatChange();
}

void Camera::resetAspectRatio() {
    _customAspectRatio = std::nullopt;
    _projMatChange();
}

Imath::V4f Camera::worldToViewportPoint(const Imath::V3f &point) {
    auto tempMat4 = projectionMatrix() * viewMatrix();
    auto tempVec = Imath::V3f(point.x, point.y, point.z);
    const auto w = tempVec.x * tempMat4[0][3] + tempVec.y * tempMat4[1][3] + tempVec.z * tempMat4[2][3] + tempMat4[3][3];
    tempMat4.multVecMatrix(tempVec, tempVec);
    
    // Transform of coordinate axis.
    return Imath::V4f((tempVec.x + 1.0) * 0.5, (1.0 - tempVec.y) * 0.5, tempVec.z, w);
}

Imath::V3f Camera::viewportToWorldPoint(const Imath::V3f &point) {
    return _innerViewportToWorldPoint(point, invViewProjMat());
}

Ray3f Camera::viewportPointToRay(const Imath::V2f &point) {
    Ray3f out;
    // Use the intersection of the near clipping plane as the origin point.
    Imath::V3f clipPoint = Imath::V3f(point.x, point.y, 0);
    out.origin = viewportToWorldPoint(clipPoint);
    // Use the intersection of the far clipping plane as the origin point.
    clipPoint.z = 1.0;
    Imath::V3f farPoint = _innerViewportToWorldPoint(clipPoint, _invViewProjMat);
    out.direction = farPoint - out.origin;
    out.direction = out.direction.normalize();
    
    return out;
}

Imath::V2f Camera::screenToViewportPoint(const Imath::V2f &point) {
    const Imath::V4f viewport = this->viewport();
    return Imath::V2f((point.x / _width - viewport.x) / viewport.z,
                  (point.y / _height - viewport.y) / viewport.w);
}

Imath::V3f Camera::screenToViewportPoint(const Imath::V3f &point) {
    const Imath::V4f viewport = this->viewport();
    return Imath::V3f((point.x / _width - viewport.x) / viewport.z,
                  (point.y / _height - viewport.y) / viewport.w, 0);
}

Imath::V2f Camera::viewportToScreenPoint(const Imath::V2f &point) {
    const Imath::V4f viewport = this->viewport();
    return Imath::V2f((viewport.x + point.x * viewport.z) * _width,
                  (viewport.y + point.y * viewport.w) * _height);
}

Imath::V3f Camera::viewportToScreenPoint(const Imath::V3f &point) {
    const Imath::V4f viewport = this->viewport();
    return Imath::V3f((viewport.x + point.x * viewport.z) * _width,
                  (viewport.y + point.y * viewport.w) * _height, 0);
}

Imath::V4f Camera::viewportToScreenPoint(const Imath::V4f &point) {
    const Imath::V4f viewport = this->viewport();
    return Imath::V4f((viewport.x + point.x * viewport.z) * _width,
                  (viewport.y + point.y * viewport.w) * _height, 0, 0);
}

Imath::V4f Camera::worldToScreenPoint(const Imath::V3f &point) {
    auto out = worldToViewportPoint(point);
    return viewportToScreenPoint(out);
}

Imath::V3f Camera::screenToWorldPoint(const Imath::V3f &point) {
    auto out = screenToViewportPoint(point);
    return viewportToWorldPoint(out);
}

Ray3f Camera::screenPointToRay(const Imath::V2f &point) {
    Imath::V2f viewportPoint = screenToViewportPoint(point);
    return viewportPointToRay(viewportPoint);
}

//void Camera::render() {
//    // compute cull frustum.
//    auto &context = engine()->_renderContext;
//    context.resetContext(scene(), this);
//    if (enableFrustumCulling && (_frustumViewChangeFlag->flag || _isFrustumProjectDirty)) {
//        _frustum.calculateFromMatrix(context.viewProjectMatrix());
//        _frustumViewChangeFlag->flag = false;
//        _isFrustumProjectDirty = false;
//    }
//
//    _updateShaderData(context);
//
//    // union scene and camera macro.
//    shaderData.mergeMacro(scene()->_globalShaderMacro, _globalShaderMacro);
//
//    _renderPipeline->render(context, cubeFace, mipLevel);
//}

void Camera::_onActive() {
    entity()->scene()->attachRenderCamera(this);
}

void Camera::_onInActive() {
    entity()->scene()->detachRenderCamera(this);
}

void Camera::_onDestroy() {
    _isInvViewProjDirty->destroy();
    _isViewMatrixDirty->destroy();
}

void Camera::_projMatChange() {
    _isFrustumProjectDirty = true;
    _isProjectionDirty = true;
    _isInvProjMatDirty = true;
    _isInvViewProjDirty->flag = true;
}

Imath::V3f Camera::_innerViewportToWorldPoint(const Imath::V3f &point, const Imath::M44f &invViewProjMat) {
    // Depth is a normalized value, 0 is nearPlane, 1 is farClipPlane.
    const auto depth = point.z * 2 - 1;
    // Transform to clipping space matrix
    Imath::V3f clipPoint = Imath::V3f(point.x * 2 - 1, 1 - point.y * 2, depth);
    invViewProjMat.multVecMatrix(clipPoint, clipPoint);
    return clipPoint;
}

//void Camera::_updateShaderData(const RenderContext &context) {
//    shaderData.setData(Camera::_viewMatrixProperty, viewMatrix());
//    shaderData.setData(Camera::_projectionMatrixProperty, projectionMatrix());
//    shaderData.setData(Camera::_vpMatrixProperty, context.viewProjectMatrix());
//    shaderData.setData(Camera::_inverseViewMatrixProperty, _transform->worldMatrix());
//    shaderData.setData(Camera::_inverseProjectionMatrixProperty, inverseProjectionMatrix());
//    shaderData.setData(Camera::_cameraPositionProperty, _transform->worldPosition());
//}

Imath::M44f Camera::invViewProjMat() {
    if (_isInvViewProjDirty->flag) {
        _isInvViewProjDirty->flag = false;
        _invViewProjMat = _transform->worldMatrix() * inverseProjectionMatrix();
    }
    return _invViewProjMat;
}

Imath::M44f Camera::inverseProjectionMatrix() {
    if (_isInvProjMatDirty) {
        _isInvProjMatDirty = false;
        _inverseProjectionMatrix = projectionMatrix().inverse();
    }
    return _inverseProjectionMatrix;
}

}
}
