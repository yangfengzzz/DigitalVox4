//
//  collision_util.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/25.
//

#ifndef collision_util_hpp
#define collision_util_hpp

#include <cassert>
#include <cmath>
#include <array>
#include "ImathVec.h"
#include "ImathPlane.h"
#include "ImathSphere.h"
#include "ray3.h"
#include "bounding_box3.h"
#include "bounding_frustum.h"

IMATH_INTERNAL_NAMESPACE_HEADER_ENTER

/**
 * Defines the intersection between a plane and a bounding volume.
 */
enum class PlaneIntersectionType {
    /** There is no intersection, the bounding volume is in the back of the plane. */
    Back,
    /** There is no intersection, the bounding volume is in the front of the plane. */
    Front,
    /** The plane is intersected. */
    Intersecting
};

/**
 * Defines how the bounding volumes intersects or contain one another.
 */
enum class ContainmentType {
    /** Indicates that there is no overlap between two bounding volumes. */
    Disjoint,
    /** Indicates that one bounding volume completely contains another volume. */
    Contains,
    /** Indicates that bounding volumes partially overlap one another. */
    Intersects
};

/**
 * Calculate the distance from a point to a plane.
 * @param plane - The plane
 * @param point - The point
 * @returns The distance from a point to a plane
 */
float distancePlaneAndPoint(const Plane3f &plane, const V3f &point);

/**
 * Get the intersection type between a plane and a point.
 * @param plane - The plane
 * @param point - The point
 * @returns The intersection type
 */
PlaneIntersectionType intersectsPlaneAndPoint(const Plane3f &plane, const V3f &point);

/**
 * Get the intersection type between a plane and a box (AABB).
 * @param plane - The plane
 * @param box - The box
 * @returns The intersection type
 */
PlaneIntersectionType intersectsPlaneAndBox(const Plane3f &plane, const BoundingBox3f &box);

/**
 * Get the intersection type between a plane and a sphere.
 * @param plane - The plane
 * @param sphere - The sphere
 * @returns The intersection type
 */
PlaneIntersectionType intersectsPlaneAndSphere(const Plane3f &plane, const Sphere3f &sphere);

/**
 * Get the intersection type between a ray and a plane.
 * @param ray - The ray
 * @param plane - The plane
 * @returns The distance from ray to plane if intersecting, -1 otherwise
 */
float intersectsRayAndPlane(const Ray3f &ray, const Plane3f &plane);

/**
 * Get the intersection type between a ray and a box (AABB).
 * @param ray - The ray
 * @param box - The box
 * @returns The distance from ray to box if intersecting, -1 otherwise
 */
float intersectsRayAndBox(const Ray3f &ray, const BoundingBox3f &box);

/**
 * Get the intersection type between a ray and a sphere.
 * @param ray - The ray
 * @param sphere - The sphere
 * @returns The distance from ray to sphere if intersecting, -1 otherwise
 */
float intersectsRayAndSphere(const Ray3f &ray, const Sphere3f &sphere);

/**
 * Check whether the boxes intersect.
 * @param boxA - The first box to check
 * @param boxB - The second box to check
 * @returns True if the boxes intersect, false otherwise
 */
bool intersectsBoxAndBox(const BoundingBox3f &boxA, const BoundingBox3f &boxB);

/**
 * Check whether the spheres intersect.
 * @param sphereA - The first sphere to check
 * @param sphereB - The second sphere to check
 * @returns True if the spheres intersect, false otherwise
 */
bool intersectsSphereAndSphere(const Sphere3f &sphereA, const Sphere3f &sphereB);

/**
 * Check whether the sphere and the box intersect.
 * @param sphere - The sphere to check
 * @param box - The box to check
 * @returns True if the sphere and the box intersect, false otherwise
 */
bool intersectsSphereAndBox(const Sphere3f &sphere, const BoundingBox3f &box);

/**
 * Get whether or not a specified bounding box intersects with this frustum (Contains or Intersects).
 * @param frustum - The frustum
 * @param box - The box
 * @returns True if bounding box intersects with this frustum, false otherwise
 */
bool intersectsFrustumAndBox(const BoundingFrustum &frustum, const BoundingBox3f &box);

/**
 * Get the containment type between a frustum and a box (AABB).
 * @param frustum - The frustum
 * @param box - The box
 * @returns The containment type
 */
ContainmentType frustumContainsBox(const BoundingFrustum &frustum, const BoundingBox3f &box);

/**
 * Get the containment type between a frustum and a sphere.
 * @param frustum - The frustum
 * @param sphere - The sphere
 * @returns The containment type
 */
ContainmentType frustumContainsSphere(const BoundingFrustum &frustum, const Sphere3f &sphere);

IMATH_INTERNAL_NAMESPACE_HEADER_EXIT

#endif /* collision_util_hpp */
