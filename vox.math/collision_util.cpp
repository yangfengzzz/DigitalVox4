//
//  collision_util.cpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/25.
//

#include "collision_util.h"
#include "ray.h"

IMATH_INTERNAL_NAMESPACE_HEADER_ENTER

float distancePlaneAndPoint(const Plane3f &plane, const V3f &point) {
    return plane.normal.dot(point) + plane.distance;
}

PlaneIntersectionType intersectsPlaneAndPoint(const Plane3f &plane, const V3f &point) {
    const auto distance = distancePlaneAndPoint(plane, point);
    if (distance > 0) {
        return PlaneIntersectionType::Front;
    }
    if (distance < 0) {
        return PlaneIntersectionType::Back;
    }
    return PlaneIntersectionType::Intersecting;
}

PlaneIntersectionType intersectsPlaneAndBox(const Plane3f &plane, const BoundingBox3f &box) {
    const auto &min = box.lowerCorner;
    const auto &max = box.upperCorner;
    const auto &normal = plane.normal;
    V3f front;
    V3f back;
    
    if (normal.x >= 0) {
        front.x = max.x;
        back.x = min.x;
    } else {
        front.x = min.x;
        back.x = max.x;
    }
    if (normal.y >= 0) {
        front.y = max.y;
        back.y = min.y;
    } else {
        front.y = min.y;
        back.y = max.y;
    }
    if (normal.z >= 0) {
        front.z = max.z;
        back.z = min.z;
    } else {
        front.z = min.z;
        back.z = max.z;
    }
    
    if (distancePlaneAndPoint(plane, front) < 0) {
        return PlaneIntersectionType::Back;
    }
    
    if (distancePlaneAndPoint(plane, back) > 0) {
        return PlaneIntersectionType::Front;
    }
    
    return PlaneIntersectionType::Intersecting;
}

PlaneIntersectionType intersectsPlaneAndSphere(const Plane3f &plane, const Sphere3f &sphere) {
    const auto &center = sphere.center;
    const auto &radius = sphere.radius;
    const auto distance = distancePlaneAndPoint(plane, center);
    if (distance > radius) {
        return PlaneIntersectionType::Front;
    }
    if (distance < -radius) {
        return PlaneIntersectionType::Back;
    }
    return PlaneIntersectionType::Intersecting;
}

float intersectsRayAndPlane(const Ray3f &ray, const Plane3f &plane) {
    const auto &normal = plane.normal;
    
    const auto dir = normal.dot(ray.direction);
    // Parallel
    if (std::abs(dir) < kEpsilonF) {
        return -1;
    }
    
    const auto position = normal.dot(ray.origin);
    auto distance = (-plane.distance - position) / dir;
    
    if (distance < 0) {
        if (distance < -kEpsilonF) {
            return -1;
        }
        
        distance = 0;
    }
    
    return distance;
}

float intersectsRayAndBox(const Ray3f &ray, const BoundingBox3f &box) {
    const auto &origin = ray.origin;
    const auto &direction = ray.direction;
    const auto &min = box.lowerCorner;
    const auto &max = box.upperCorner;
    const auto &dirX = direction.x;
    const auto &dirY = direction.y;
    const auto &dirZ = direction.z;
    const auto &oriX = origin.x;
    const auto &oriY = origin.y;
    const auto &oriZ = origin.z;
    float distance = 0;
    float tmax = std::numeric_limits<float>::max();
    
    if (std::abs(dirX) < kEpsilonF) {
        if (oriX < min.x || oriX > max.x) {
            return -1;
        }
    } else {
        const float inverse = 1.0 / dirX;
        float t1 = (min.x - oriX) * inverse;
        float t2 = (max.x - oriX) * inverse;
        
        if (t1 > t2) {
            const auto temp = t1;
            t1 = t2;
            t2 = temp;
        }
        
        distance = std::max(t1, distance);
        tmax = std::min(t2, tmax);
        
        if (distance > tmax) {
            return -1;
        }
    }
    
    if (std::abs(dirY) < kEpsilonF) {
        if (oriY < min.y || oriY > max.y) {
            return -1;
        }
    } else {
        const float inverse = 1.0 / dirY;
        float t1 = (min.y - oriY) * inverse;
        float t2 = (max.y - oriY) * inverse;
        
        if (t1 > t2) {
            const auto temp = t1;
            t1 = t2;
            t2 = temp;
        }
        
        distance = std::max(t1, distance);
        tmax = std::min(t2, tmax);
        
        if (distance > tmax) {
            return -1;
        }
    }
    
    if (std::abs(dirZ) < kEpsilonF) {
        if (oriZ < min.z || oriZ > max.z) {
            return -1;
        }
    } else {
        const float inverse = 1.0 / dirZ;
        float t1 = (min.z - oriZ) * inverse;
        float t2 = (max.z - oriZ) * inverse;
        
        if (t1 > t2) {
            const auto temp = t1;
            t1 = t2;
            t2 = temp;
        }
        
        distance = std::max(t1, distance);
        tmax = std::min(t2, tmax);
        
        if (distance > tmax) {
            return -1;
        }
    }
    
    return distance;
}

float intersectsRayAndSphere(const Ray3f &ray, const Sphere3f &sphere) {
    const auto &origin = ray.origin;
    const auto &direction = ray.direction;
    const auto &center = sphere.center;
    const auto &radius = sphere.radius;
    
    V3f m = origin - center;
    const auto b = m.dot(direction);
    const auto c = m.length2() - radius * radius;
    
    if (b > 0 && c > 0) {
        return -1;
    }
    
    float discriminant = b * b - c;
    if (discriminant < 0) {
        return -1;
    }
    
    float distance = -b - std::sqrt(discriminant);
    if (distance < 0) {
        distance = 0;
    }
    
    return distance;
}

bool intersectsBoxAndBox(const BoundingBox3f &boxA, const BoundingBox3f &boxB) {
    if (boxA.lowerCorner.x > boxB.upperCorner.x || boxB.lowerCorner.x > boxA.upperCorner.x) {
        return false;
    }
    
    if (boxA.lowerCorner.y > boxB.upperCorner.y || boxB.lowerCorner.y > boxA.upperCorner.y) {
        return false;
    }
    
    return !(boxA.lowerCorner.z > boxB.upperCorner.z || boxB.lowerCorner.z > boxA.upperCorner.z);
}

bool intersectsSphereAndSphere(const Sphere3f &sphereA, const Sphere3f &sphereB) {
    const auto radiisum = sphereA.radius + sphereB.radius;
    return (sphereA.center - sphereB.center).length2() < radiisum * radiisum;
}

bool intersectsSphereAndBox(const Sphere3f &sphere, const BoundingBox3f &box) {
    const auto &center = sphere.center;
    const auto &max = box.upperCorner;
    const auto &min = box.lowerCorner;
    
    V3f closestPoint = V3f(std::max(min.x, std::min(center.x, max.x)),
                           std::max(min.y, std::min(center.y, max.y)),
                           std::max(min.z, std::min(center.z, max.z)));
    
    const auto distance = (center - closestPoint).length2();
    return distance <= sphere.radius * sphere.radius;
}

bool intersectsFrustumAndBox(const BoundingFrustum &frustum, const BoundingBox3f &box) {
    const auto &min = box.lowerCorner;
    const auto &max = box.upperCorner;
    V3f back;
    
    for (int i = 0; i < 6; ++i) {
        const auto plane = frustum.getPlane(i);
        const auto &normal = plane.normal;
        
        back.x = normal.x >= 0 ? min.x : max.x;
        back.y = normal.y >= 0 ? min.y : max.y;
        back.z = normal.z >= 0 ? min.z : max.z;
        if (plane.normal.dot(back) > -plane.distance) {
            return false;
        }
    }
    
    return true;
}

ContainmentType frustumContainsBox(const BoundingFrustum &frustum, const BoundingBox3f &box) {
    const auto &min = box.lowerCorner;
    const auto &max = box.upperCorner;
    V3f front;
    V3f back;
    auto result = ContainmentType::Contains;
    
    for (int i = 0; i < 6; ++i) {
        const auto plane = frustum.getPlane(i);
        const auto &normal = plane.normal;
        
        if (normal.x >= 0) {
            front.x = max.x;
            back.x = min.x;
        } else {
            front.x = min.x;
            back.x = max.x;
        }
        if (normal.y >= 0) {
            front.y = max.y;
            back.y = min.y;
        } else {
            front.y = min.y;
            back.y = max.y;
        }
        if (normal.z >= 0) {
            front.z = max.z;
            back.z = min.z;
        } else {
            front.z = min.z;
            back.z = max.z;
        }
        
        if (intersectsPlaneAndPoint(plane, back) == PlaneIntersectionType::Front) {
            return ContainmentType::Disjoint;
        }
        
        if (intersectsPlaneAndPoint(plane, front) == PlaneIntersectionType::Front) {
            result = ContainmentType::Intersects;
        }
    }
    
    return result;
}

ContainmentType frustumContainsSphere(const BoundingFrustum &frustum, const Sphere3f &sphere) {
    auto result = ContainmentType::Contains;
    
    for (int i = 0; i < 6; ++i) {
        const auto plane = frustum.getPlane(i);
        const auto intersectionType = intersectsPlaneAndSphere(plane, sphere);
        if (intersectionType == PlaneIntersectionType::Front) {
            return ContainmentType::Disjoint;
        } else if (intersectionType == PlaneIntersectionType::Intersecting) {
            result = ContainmentType::Intersects;
            break;
        }
    }
    
    return result;
}

IMATH_INTERNAL_NAMESPACE_HEADER_EXIT
