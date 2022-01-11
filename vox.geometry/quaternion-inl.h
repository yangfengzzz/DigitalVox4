// Copyright (c) 2018 Doyub Kim
//
// I am making my contributions/submissions to this project solely in my
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#ifndef INCLUDE_JET_DETAIL_QUATERNION_INL_H_
#define INCLUDE_JET_DETAIL_QUATERNION_INL_H_

#include <limits>

namespace jet {

// Constructors
template <typename T>
inline Quaternion<T>::Quaternion() {
    setIdentity();
}

template <typename T>
inline Quaternion<T>::Quaternion(T newW, T newX, T newY, T newZ) {
    set(newW, newX, newY, newZ);
}

template <typename T>
inline Quaternion<T>::Quaternion(const std::initializer_list<T>& lst) {
    set(lst);
}

template <typename T>
inline Quaternion<T>::Quaternion(const Vector3<T>& axis, T angle) {
    set(axis, angle);
}

template <typename T>
inline Quaternion<T>::Quaternion(const Vector3<T>& from, const Vector3<T>& to) {
    set(from, to);
}

template <typename T>
inline Quaternion<T>::Quaternion(const Vector3<T>& rotationBasis0,
                                 const Vector3<T>& rotationBasis1,
                                 const Vector3<T>& rotationBasis2) {
    set(rotationBasis0, rotationBasis1, rotationBasis2);
}

template <typename T>
inline Quaternion<T>::Quaternion(const Eigen::Matrix<T, 3, 3>& matrix) {
    set(matrix);
}

template <typename T>
inline Quaternion<T>::Quaternion(const Quaternion& other) {
    set(other);
}


// Basic setters
template <typename T>
inline void Quaternion<T>::set(const Quaternion& other) {
    value = other.value;
}

template <typename T>
inline void Quaternion<T>::set(T newW, T newX, T newY, T newZ) {
    value = Eigen::Quaternion<T>(newW, newX, newY, newZ);
}

template <typename T>
inline void Quaternion<T>::set(const std::initializer_list<T>& lst) {
    assert(lst.size() == 4);
    
    auto inputElem = lst.begin();
    value = Eigen::Quaternion<T>(static_cast<T>(*inputElem),
                                 static_cast<T>(*(++inputElem)),
                                 static_cast<T>(*(++inputElem)),
                                 static_cast<T>(*(++inputElem)));
}

template <typename T>
inline void Quaternion<T>::set(const Vector3<T>& axis, T angle) {
    static const T eps = std::numeric_limits<T>::epsilon();
    
    T axisLengthSquared = axis.lengthSquared();
    
    if (axisLengthSquared < eps) {
        setIdentity();
    } else {
        Vector3<T> normalizedAxis = axis.normalized();
        T s = std::sin(angle / 2);
        
        value = Eigen::Quaternion<T>(std::cos(angle / 2),
                                     normalizedAxis.x() * s,
                                     normalizedAxis.y() * s,
                                     normalizedAxis.z() * s);
    }
}

template <typename T>
inline void Quaternion<T>::set(const Vector3<T>& from, const Vector3<T>& to) {
    static const T eps = std::numeric_limits<T>::epsilon();
    
    Vector3<T> axis = from.cross(to);
    
    T fromLengthSquared = from.lengthSquared();
    T toLengthSquared = to.lengthSquared();
    
    if (fromLengthSquared < eps ||
        toLengthSquared < eps) {
        setIdentity();
    } else {
        T axisLengthSquared = axis.lengthSquared();
        
        // In case two vectors are exactly the opposite, pick orthogonal vector
        // for axis.
        if (axisLengthSquared < eps) {
            axis = std::get<0>(from.tangential());
        }
        
        set(from.dot(to), axis.x(), axis.y(), axis.z());
        value.w() += l2Norm();
        
        normalize();
    }
}

template <typename T>
inline void Quaternion<T>::set(const Vector3<T>& rotationBasis0,
                               const Vector3<T>& rotationBasis1,
                               const Vector3<T>& rotationBasis2) {
    Eigen::Matrix<T, 3, 3> matrix3;
    matrix3.col(0) = rotationBasis0.normalized().value;
    matrix3.col(1) = rotationBasis1.normalized().value;
    matrix3.col(2) = rotationBasis2.normalized().value;
    
    set(matrix3);
}

template <typename T>
inline void Quaternion<T>::set(const Eigen::Matrix<T, 3, 3>& m) {
    static const T eps = std::numeric_limits<T>::epsilon();
    static const T quater = static_cast<T>(0.25);
    
    T onePlusTrace = m.trace() + 1;
    
    if (onePlusTrace > eps) {
        T S = std::sqrt(onePlusTrace) * 2;
        value = Eigen::Quaternion<T>(quater * S,
                                     (m(2, 1) - m(1, 2)) / S,
                                     (m(0, 2) - m(2, 0)) / S,
                                     (m(1, 0) - m(0, 1)) / S);
    } else if (m(0, 0) > m(1, 1) && m(0, 0) > m(2, 2)) {
        T S = std::sqrt(1 + m(0, 0) - m(1, 1) - m(2, 2)) * 2;
        
        value = Eigen::Quaternion<T>((m(2, 1) - m(1, 2)) / S,
                                     quater * S,
                                     (m(0, 1) + m(1, 0)) / S,
                                     (m(0, 2) + m(2, 0)) / S);
    } else if (m(1, 1) > m(2, 2)) {
        T S = std::sqrt(1 + m(1, 1) - m(0, 0) - m(2, 2)) * 2;
        
        value = Eigen::Quaternion<T>((m(0, 2) - m(2, 0)) / S,
                                     (m(0, 1) + m(1, 0)) / S,
                                     quater * S,
                                     (m(1, 2) + m(2, 1)) / S);
    } else {
        T S = std::sqrt(1 + m(2, 2) - m(0, 0) - m(1, 1)) * 2;
        
        value = Eigen::Quaternion<T>((m(1, 0) - m(0, 1)) / S,
                                     (m(0, 2) + m(2, 0)) / S,
                                     (m(1, 2) + m(2, 1)) / S,
                                     quater * S);
    }
}


// Basic getters
template <typename T>
template <typename U>
Quaternion<U> Quaternion<T>::castTo() const {
    return Quaternion<U>(static_cast<U>(w()),
                         static_cast<U>(x()),
                         static_cast<U>(y()),
                         static_cast<U>(z()));
}

//! Returns normalized quaternion.
template <typename T>
Quaternion<T> Quaternion<T>::normalized() const {
    Quaternion q(*this);
    q.normalize();
    return q;
}

// Binary operator methods - new instance = this instance (+) input
template <typename T>
inline Vector3<T> Quaternion<T>::mul(const Vector3<T>& v) const {
    return Vector3<T>(value._transformVector(v.value));
}

template <typename T>
inline Quaternion<T> Quaternion<T>::mul(const Quaternion& other) const {
    return Quaternion(value * other.value);
}

template <typename T>
inline T Quaternion<T>::dot(const Quaternion<T>& other) {
    return value.dot(other.value);
}

// Binary operator methods - new instance = input (+) this instance
template <typename T>
inline Quaternion<T> Quaternion<T>::rmul(const Quaternion& other) const {
    return Quaternion(other.value * value);
}


// Augmented operator methods - this instance (+)= input
template <typename T>
inline void Quaternion<T>::imul(const Quaternion& other) {
    *this = mul(other);
}


// Modifiers
template <typename T>
inline void Quaternion<T>::setIdentity() {
    set(1, 0, 0, 0);
}

template <typename T>
inline void Quaternion<T>::rotate(T angleInRadians) {
    Vector3<T> axis;
    T currentAngle;
    
    getAxisAngle(&axis, &currentAngle);
    
    currentAngle += angleInRadians;
    
    set(axis, currentAngle);
}

template <typename T>
inline void Quaternion<T>::normalize() {
    value.normalize();
}


// Complex getters
template <typename T>
inline Vector3<T> Quaternion<T>::axis() const {
    return Vector3<T>(Eigen::AngleAxis<T>(value).axis());
}

template <typename T>
inline T Quaternion<T>::angle() const {
    return Eigen::AngleAxis<T>(value).angle();
}

template <typename T>
inline void Quaternion<T>::getAxisAngle(Vector3<T>* axis, T* angle) const {
    auto angleAxis = Eigen::AngleAxis<T>(value);
    axis->value = angleAxis.axis();
    *angle = angleAxis.angle();
}

template <typename T>
inline Quaternion<T> Quaternion<T>::inverse() const {
    return Quaternion(value.inverse());
}

template <typename T>
inline Eigen::Matrix<T, 3, 3> Quaternion<T>::matrix3() const {
    return value.toRotationMatrix();
}

template <typename T>
inline Eigen::Matrix<T, 4, 4> Quaternion<T>::matrix4() const {
    Eigen::Matrix<T, 4, 4> mat;
    mat.template block<3, 3>(0, 0) = value.toRotationMatrix();
    return mat;
}

template <typename T>
inline T Quaternion<T>::l2Norm() const {
    return value.norm();
}

// Setter operators
template <typename T>
inline Quaternion<T>& Quaternion<T>::operator=(const Quaternion& other) {
    set(other);
    return *this;
}

template <typename T>
inline Quaternion<T>& Quaternion<T>::operator*=(const Quaternion& other) {
    imul(other);
    return *this;
}


// Getter operators
template <typename T>
inline T& Quaternion<T>::operator[](size_t i) {
    return value.coeffs()[i];
}

template <typename T>
inline const T& Quaternion<T>::operator[](size_t i) const {
    return (&w)[i];
}

template <typename T>
bool Quaternion<T>::operator==(const Quaternion& other) const {
    return value == other.value;
}

template <typename T>
bool Quaternion<T>::operator!=(const Quaternion& other) const {
    return value != other.value;
}

template <typename T>
Quaternion<T> Quaternion<T>::makeIdentity() {
    return Quaternion();
}


template <typename T>
inline Quaternion<T> slerp(const Quaternion<T>& a,
                           const Quaternion<T>& b,
                           T t) {
    static const double threshold = 0.01;
    static const T eps = std::numeric_limits<T>::epsilon();
    
    T cosHalfAngle = dot(a, b);
    T weightA, weightB;
    
    // For better accuracy, return lerp result when a and b are close enough.
    if (1.0 - std::fabs(cosHalfAngle) < threshold) {
        weightA = 1.0 - t;
        weightB = t;
    } else {
        T halfAngle = std::acos(cosHalfAngle);
        T sinHalfAngle = std::sqrt(1 - cosHalfAngle * cosHalfAngle);
        
        // In case of angle ~ 180, pick middle value.
        // If not, perform slerp.
        if (std::fabs(sinHalfAngle) < eps) {
            weightA = static_cast<T>(0.5);
            weightB = static_cast<T>(0.5);
        } else {
            weightA = std::sin((1 - t) * halfAngle) / sinHalfAngle;
            weightB = std::sin(t * halfAngle) / sinHalfAngle;
        }
    }
    
    return Quaternion<T>(weightA * a.w + weightB * b.w,
                         weightA * a.x + weightB * b.x,
                         weightA * a.y + weightB * b.y,
                         weightA * a.z + weightB * b.z);
}


// Operator overloadings
template <typename T>
inline Vector<T, 3> operator*(const Quaternion<T>& q, const Vector<T, 3>& v) {
    return q.mul(v);
}

template <typename T>
inline Quaternion<T> operator*(const Quaternion<T>& a, const Quaternion<T>& b) {
    return a.mul(b);
}

}  // namespace jet

#endif  // INCLUDE_JET_DETAIL_QUATERNION_INL_H_
