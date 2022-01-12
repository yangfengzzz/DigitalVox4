//
// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the OpenEXR Project.
//

//
// Convenience functions that call GLU with Imath types
//

#ifndef INCLUDED_IMATHGLU_H
#define INCLUDED_IMATHGLU_H

#include "ImathVec.h"
#include "ImathMatrix.h"

IMATH_INTERNAL_NAMESPACE_HEADER_ENTER

/**
 * Calculate a right-handed look-at matrix.
 * @param eye - The position of the viewer's eye
 * @param target - The camera look-at target
 * @param up - The camera's up vector
 * @return out - The calculated look-at matrix
 */
inline M44f lookAt(const V3f &eye, const V3f &target, const V3f &up) {
    V3f zAxis = eye - target;
    zAxis.normalize();
    V3f xAxis = up.cross(zAxis);
    xAxis.normalize();
    V3f yAxis = zAxis.cross(xAxis);
    
    return M44f(xAxis.x,
                yAxis.x,
                zAxis.x,
                0,
                
                xAxis.y,
                yAxis.y,
                zAxis.y,
                0,
                
                xAxis.z,
                yAxis.z,
                zAxis.z,
                0,
                
                -xAxis.dot(eye),
                -yAxis.dot(eye),
                -zAxis.dot(eye),
                1);
}

/**
 * Calculate an orthographic projection matrix.
 * @param left - The left edge of the viewing
 * @param right - The right edge of the viewing
 * @param bottom - The bottom edge of the viewing
 * @param top - The top edge of the viewing
 * @param near - The depth of the near plane
 * @param far - The depth of the far plane
 * @return out - The calculated orthographic projection matrix
 * @remarks Metal NDC z:[0,1]
 */
inline M44f ortho(float left, float right, float bottom, float top, float near, float far) {
    auto lr = 1 / (left - right);
    auto bt = 1 / (bottom - top);
    auto nf = 1 / (near - far);
    
    return M44f(-2 * lr,
                  0,
                  0,
                  0,
                  
                  0,
                  -2 * bt,
                  0,
                  0,
                  
                  0,
                  0,
                  nf,
                  0,
                  
                  (left + right) * lr,
                  (top + bottom) * bt,
                  near * nf,
                  1);
}

/**
 * Calculate a perspective projection matrix.
 * @param fovy - Field of view in the y direction, in radians
 * @param aspect - Aspect ratio, defined as view space width divided by height
 * @param near - The depth of the near plane
 * @param far - The depth of the far plane
 * @return out - The calculated perspective projection matrix
 * @remarks Metal NDC z:[0,1]
 */
inline M44f perspective(float fovy, float aspect, float near, float far) {
    auto f = 1.0 / std::tan(fovy / 2);
    auto nf = 1 / (near - far);
    
    return M44f(f / aspect,
                  0,
                  0,
                  0,
                  
                  0,
                  f,
                  0,
                  0,
                  
                  0,
                  0,
                  far * nf,
                  -1,
                  
                  0,
                  0,
                  far * near * nf,
                  0);
}

IMATH_INTERNAL_NAMESPACE_HEADER_EXIT

#endif
