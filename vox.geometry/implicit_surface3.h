//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef INCLUDE_VOX_IMPLICIT_SURFACE3_H_
#define INCLUDE_VOX_IMPLICIT_SURFACE3_H_

#include "surface3.h"

namespace vox {

//! Abstract base class for 3-D implicit surface.
class ImplicitSurface3 : public Surface3 {
public:
    //! Default constructor.
    ImplicitSurface3(const Transform3D &transform = Transform3D(),
                     bool isNormalFlipped = false);
    
    //! Copy constructor.
    ImplicitSurface3(const ImplicitSurface3 &other);
    
    //! Default destructor.
    virtual ~ImplicitSurface3();
    
    //! Returns signed distance from the given point \p otherPoint.
    double signedDistance(const Point3D &otherPoint) const;
    
protected:
    //! Returns signed distance from the given point \p otherPoint in local
    //! space.
    virtual double signedDistanceLocal(const Point3D &otherPoint) const = 0;
    
private:
    double closestDistanceLocal(const Point3D &otherPoint) const override;
    
    bool isInsideLocal(const Point3D &otherPoint) const override;
};

//! Shared pointer type for the ImplicitSurface3.
using ImplicitSurface3Ptr = std::shared_ptr<ImplicitSurface3>;

}  // namespace vox

#endif  // INCLUDE_VOX_IMPLICIT_SURFACE3_H_
