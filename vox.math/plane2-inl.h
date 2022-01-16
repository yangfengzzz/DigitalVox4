//
//  plane2-inl.h
//  DigitalVox4
//
//  Created by 杨丰 on 2022/1/16.
//

#ifndef plane2_inl_h
#define plane2_inl_h

namespace vox {
template <typename T>
Plane<T, 2>::Plane() : Plane(Vector2<T>(), 0)  {
    
}

template <typename T>
Plane<T, 2>::Plane(const Vector2<T>& newNormal, const T& newDistance):
normal(newNormal),
distance(newDistance){
}

template <typename T>
Plane<T, 2>::Plane(const Vector2<T>& point0, const Vector2<T>& point1, const Vector2<T>& point2) {
    // TODO
}

template <typename T>
Plane<T, 2>::Plane(const Plane& other):
normal(other.normal),
distance(other.distance) {
}

template <typename T>
Plane<T, 2> Plane<T, 2>::normalized() const {
    T factor = 1.0 / normal.length();

    Vector2<T> outNormal(outNormal.x = normal.x * factor,
                         outNormal.y = normal.y * factor);
    return Plane<T, 2>(outNormal, distance * factor);
}

template <typename T>
void Plane<T, 2>::normalize() {
    T factor = 1.0 / normal.length();

    normal.x *= factor;
    normal.y *= factor;
    normal.z *= factor;
    distance *= factor;
}

}

#endif /* plane2_inl_h */
