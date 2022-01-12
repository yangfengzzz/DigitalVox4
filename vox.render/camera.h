//
//  camera.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/12.
//

#ifndef camera_hpp
#define camera_hpp

#include "component.h"
#include "shader/shader_data.h"
#include "layer.h"
#include "ray3.h"
#include "ImathMatrix.h"
#include "bounding_frustum.h"
#include "updateFlag.h"

namespace vox {
namespace sg {
/**
 * Camera clear flags enumeration.
 */
enum class CameraClearFlags {
    /* Clear depth and color from background. */
    DepthColor,
    /* Clear depth only. */
    Depth,
    /* Do nothing. */
    None
};

/**
 * Camera component, as the entrance to the three-dimensional world.
 */
class Camera : public Component {
public:
    /** Shader data. */
    ShaderData shaderData = ShaderData();
    
    /** Rendering priority - A Camera with higher priority will be rendered on top of a camera with lower priority. */
    int priority = 0;
    
    /** Whether to enable frustum culling, it is enabled by default. */
    bool enableFrustumCulling = true;
    
    /**
     * Determining what to clear when rendering by a Camera.
     * @defaultValue `CameraClearFlags.DepthColor`
     */
    CameraClearFlags clearFlags = CameraClearFlags::DepthColor;
    
    /**
     * Culling mask - which layers the camera renders.
     * @remarks Support bit manipulation, corresponding to Entity's layer.
     */
    Layer cullingMask = Layer::Everything;
    
    /**
     * Create the Camera component.
     * @param entity - Entity
     */
    Camera(Entity *entity);
    
    /**
     * Near clip plane - the closest point to the camera when rendering occurs.
     */
    float nearClipPlane() const;
    
    void setNearClipPlane(float value);
    
    /**
     * Far clip plane - the furthest point to the camera when rendering occurs.
     */
    float farClipPlane() const;
    
    void setFarClipPlane(float value);
    
    /**
     * The camera's view angle. activating when camera use perspective projection.
     */
    float fieldOfView() const;
    
    void setFieldOfView(float value);
    
    /**
     * Aspect ratio. The default is automatically calculated by the viewport's aspect ratio. If it is manually set,
     * the manual value will be kept. Call resetAspectRatio() to restore it.
     */
    float aspectRatio() const;
    
    void setAspectRatio(float value);
    
    /**
     * Viewport, normalized expression, the upper left corner is (0, 0), and the lower right corner is (1, 1).
     * @remarks Re-assignment is required after modification to ensure that the modification takes effect.
     */
    Imath::V4f viewport() const;
    
    void setViewport(const Imath::V4f &value);
    
    /**
     * Whether it is orthogonal, the default is false. True will use orthographic projection, false will use perspective projection.
     */
    bool isOrthographic() const;
    
    void setIsOrthographic(bool value);
    
    /**
     * Half the size of the camera in orthographic mode.
     */
    float orthographicSize() const;
    
    void setOrthographicSize(float value);
    
    /**
     * View matrix.
     */
    Imath::M44f viewMatrix();
    
    /**
     * The projection matrix is ​​calculated by the relevant parameters of the camera by default.
     * If it is manually set, the manual value will be maintained. Call resetProjectionMatrix() to restore it.
     */
    void setProjectionMatrix(const Imath::M44f &value);
    
    Imath::M44f projectionMatrix();
    
    /**
     * Whether to enable HDR.
     * @todo When render pipeline modification
     */
    bool enableHDR();
    
    void setEnableHDR(bool value);
    
public:
    /**
     * Restore the automatic calculation of projection matrix through fieldOfView, nearClipPlane and farClipPlane.
     */
    void resetProjectionMatrix();
    
    /**
     * Restore the automatic calculation of the aspect ratio through the viewport aspect ratio.
     */
    void resetAspectRatio();
    
    /**
     * Transform a point from world space to viewport space.
     * @param point - Point in world space
     * @return out - A point in the viewport space, X and Y are the viewport space coordinates,
     * Z is the viewport depth, the near clipping plane is 0, the far clipping plane is 1, and W is the world unit distance from the camera
     */
    Imath::V4f worldToViewportPoint(const Imath::V3f &point);
    
    /**
     * Transform a point from viewport space to world space.
     * @param point - Point in viewport space, X and Y are the viewport space coordinates,
     * Z is the viewport depth. The near clipping plane is 0, and the far clipping plane is 1
     * @returns Point in world space
     */
    Imath::V3f viewportToWorldPoint(const Imath::V3f &point);
    
    /**
     * Generate a ray by a point in viewport.
     * @param point - Point in viewport space, which is represented by normalization
     * @returns Ray
     */
    Imath::Ray3f viewportPointToRay(const Imath::V2f &point);
    
    /**
     * Transform the X and Y coordinates of a point from screen space to viewport space
     * @param point - Point in screen space
     * @returns Point in viewport space
     */
    Imath::V2f screenToViewportPoint(const Imath::V2f &point);
    
    Imath::V3f screenToViewportPoint(const Imath::V3f &point);
    
    /**
     * Transform the X and Y coordinates of a point from viewport space to screen space.
     * @param point - Point in viewport space
     * @returns Point in screen space
     */
    Imath::V2f viewportToScreenPoint(const Imath::V2f &point);
    
    Imath::V3f viewportToScreenPoint(const Imath::V3f &point);
    
    Imath::V4f viewportToScreenPoint(const Imath::V4f &point);
    
    /**
     * Transform a point from world space to screen space.
     * @param point - Point in world space
     * @returns Point of screen space
     */
    Imath::V4f worldToScreenPoint(const Imath::V3f &point);
    
    /**
     * Transform a point from screen space to world space.
     * @param point - Screen space point
     * @returns Point in world space
     */
    Imath::V3f screenToWorldPoint(const Imath::V3f &point);
    
    /**
     * Generate a ray by a point in screen.
     * @param point - Point in screen space, the unit is pixel
     * @returns Ray
     */
    Imath::Ray3f screenPointToRay(const Imath::V2f &point);
    
    /**
     * Manually call the rendering of the camera.
     */
    void render();
    
    void resize(uint32_t width, uint32_t height);
    
public:
    void _onActive() override;
    
    void _onInActive() override;
    
    void _onDestroy() override;
    
private:
    void _projMatChange();
    
    Imath::V3f _innerViewportToWorldPoint(const Imath::V3f &point, const Imath::M44f &invViewProjMat);
    
//    void _updateShaderData(const RenderContext &context);
    
    /**
     * The inverse matrix of view projection matrix.
     */
    Imath::M44f invViewProjMat();
    
    /**
     * The inverse of the projection matrix.
     */
    Imath::M44f inverseProjectionMatrix();
    
    ShaderProperty _viewMatrixProperty;
    ShaderProperty _projectionMatrixProperty;
    ShaderProperty _vpMatrixProperty;
    ShaderProperty _inverseViewMatrixProperty;
    ShaderProperty _inverseProjectionMatrixProperty;
    ShaderProperty _cameraPositionProperty;
    
    ShaderMacroCollection _globalShaderMacro = ShaderMacroCollection();
    Imath::BoundingFrustum _frustum = Imath::BoundingFrustum();
    
    bool _isOrthographic = false;
    bool _isProjMatSetting = false;
    float _nearClipPlane = 0.1;
    float _farClipPlane = 100;
    float _fieldOfView = 45;
    float _orthographicSize = 10;
    bool _isProjectionDirty = true;
    bool _isInvProjMatDirty = true;
    bool _isFrustumProjectDirty = true;
    std::optional<float> _customAspectRatio = std::nullopt;
    
    std::unique_ptr<UpdateFlag> _frustumViewChangeFlag;
    Transform *_transform;
    std::unique_ptr<UpdateFlag> _isViewMatrixDirty;
    std::unique_ptr<UpdateFlag> _isInvViewProjDirty;
    Imath::M44f _projectionMatrix = Imath::M44f();
    Imath::M44f _viewMatrix = Imath::M44f();
    Imath::V4f _viewport = Imath::V4f(0, 0, 1, 1);
    Imath::M44f _inverseProjectionMatrix = Imath::M44f();
    Imath::V2f _lastAspectSize = Imath::V2f();
    Imath::M44f _invViewProjMat = Imath::M44f();
    
    uint32_t _width;
    uint32_t _height;
};


}
}

#endif /* camera_hpp */
