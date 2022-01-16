//
//  matrix_utils.h
//  DigitalVox4
//
//  Created by 杨丰 on 2022/1/16.
//

#ifndef matrix_utils_h
#define matrix_utils_h

#include "matrix4x4.h"
#include "quaternion.h"

namespace vox {
//! Makes scale matrix.
template <typename T>
inline Matrix<T, 4, 4> makeScaleMatrix(T sx, T sy, T sz) {
    return Matrix(sx, 0, 0, 0,
                  0, sy, 0, 0,
                  0, 0, sz, 0,
                  0, 0, 0, 1);
}

//! Makes scale matrix.
template <typename T>
inline Matrix<T, 4, 4> makeScaleMatrix(const Vector3<T>& s) {
    return makeScaleMatrix(s.x, s.y, s.z);
}

//! Makes rotation matrix.
//! \warning Input angle should be radian.
template <typename T>
inline Matrix<T, 4, 4> makeRotationMatrix(const Vector3<T>& axis, T rad) {
    return Matrix(Matrix<T, 3, 3>::makeRotationMatrix(axis, rad));
}

//! Makes translation matrix.
template <typename T>
inline Matrix<T, 4, 4> makeTranslationMatrix(const Vector3<T>& t) {
    return Matrix(1, 0, 0, 0,
                  0, 1, 0, 0,
                  0, 0, 1, 0,
                  t.x, t.y, t.z, 1);
}

//! Makes rotation && translation matrix.
template <typename T>
inline Matrix<T, 4, 4> makeRotationTranslationMatrix(const Quaternion<T>& q, const Vector3<T>& t) {
    auto mat = q.matrix4();
    mat[12] = t.x;
    mat[13] = t.y;
    mat[14] = t.z;
    return mat;
}

//! Makes affine matrix.
template <typename T>
inline Matrix<T, 4, 4> makeAffineMatrix(const Vector3<T>& s, const Quaternion<T>& q, const Vector3<T>& t) {
    T x = q.x;
    T y = q.y;
    T z = q.z;
    T w = q.w;
    
    T x2 = x + x;
    T y2 = y + y;
    T z2 = z + z;
    
    T xx = x * x2;
    T xy = x * y2;
    T xz = x * z2;
    T yy = y * y2;
    T yz = y * z2;
    T zz = z * z2;
    T wx = w * x2;
    T wy = w * y2;
    T wz = w * z2;
    T sx = s.x;
    T sy = s.y;
    T sz = s.z;
    
    return Matrix<T, 4, 4>((1 - (yy + zz)) * sx,
                           (xy + wz) * sx,
                           (xz - wy) * sx,
                           0,
                           
                           (xy - wz) * sy,
                           (1 - (xx + zz)) * sy,
                           (yz + wx) * sy,
                           0,
                           
                           (xz + wy) * sz,
                           (yz - wx) * sz,
                           (1 - (xx + yy)) * sz,
                           0,
                           
                           t.x,
                           t.y,
                           t.z,
                           1);
}

}

#endif /* matrix_utils_h */
