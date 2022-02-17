//
//  animation_field.hpp
//  vox.sph
//
//  Created by 杨丰 on 2022/2/17.
//

#ifndef animation_field_hpp
#define animation_field_hpp

#include "vector3.h"
#include "matrix3x3.h"
#include <string>

namespace vox {
namespace sph {
class AnimationField {
public:
    AnimationField(
                   const std::string &particleFieldName,
                   const Vector3F &pos, const Matrix3x3F & rotation, const Vector3F &scale,
                   const std::string expression[3], const unsigned int type = 0);
    virtual ~AnimationField();
    
protected:
    std::string m_particleFieldName;
    Vector3F m_x;
    Matrix3x3F m_rotation;
    Vector3F m_scale;
    std::string m_expression[3];
    unsigned int m_type;
    float m_startTime;
    float m_endTime;
    
    inline bool inBox(const Vector3F &x, const Vector3F &xBox, const Matrix3x3F &rotBox, const Vector3F &scaleBox) {
        const Vector3F xlocal = rotBox.transposed() * (x - xBox);
        // for a box shape, m_scale stores the half-size of the box
        // inside box if closer than half-size on all axes
        return std::abs(xlocal.x) < scaleBox.x && std::abs(xlocal.y) < scaleBox.y && std::abs(xlocal.z) < scaleBox.z;
    }
    
    inline bool inCylinder(const Vector3F &x, const Vector3F &xCyl, const Matrix3x3F &rotCyl, const float h, const float r2) {
        const Vector3F xlocal = rotCyl.transposed() * (x - xCyl);
        // inside cylinder if distance to x-axis is less than r
        // and projection on x-axis is between 0 and h
        const float proj = xlocal.x;
        const float d2 = Vector2F(xlocal.y, xlocal.z).lengthSquared();
        const float hHalf = static_cast<float>(0.5)*h;
        return (proj > -hHalf) && (proj < hHalf) && (d2 < r2);
    }
    
    inline bool inSphere(const Vector3F &x, const Vector3F &pos, const Matrix3x3F &rot, const float radius) {
        const Vector3F xlocal = rot.transposed() * (x - pos);
        return (xlocal.length() < radius);
    }
    
    inline bool inShape(const int type, const Vector3F &x, const Vector3F &pos, const Matrix3x3F &rot, const Vector3F &scale) {
        if (type == 1)
            return inSphere(x, pos, rot, scale[0]);
        else if (type == 2) {
            const float h = scale[0];
            const float r = scale[1];
            return inCylinder(x, pos, rot, h, r*r);
        }
        else
            return inBox(x, pos, rot, scale * 0.5f);
    }
    
public:
    void setStartTime(float val) { m_startTime = val; }
    void setEndTime(float val) { m_endTime = val; }
    
    void step();
    virtual void reset();
    
};


}
}
#endif /* animation_field_hpp */
