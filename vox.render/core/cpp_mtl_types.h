//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cpp_mtl_Types_hpp
#define cpp_mtl_Types_hpp

#include <cstdint>
#include <climits>

namespace MTL {
using UInteger = unsigned long;
static const UInteger UIntegerMax = ULONG_MAX;

struct Range {
    UInteger location;
    UInteger length;
};

//MARK: - Origin
struct Origin {
    UInteger x, y, z;
};

inline Origin originMake(UInteger x, UInteger y, UInteger z) {
    Origin origin = {x, y, z};
    return origin;
}

//MARK: - Size
struct Size {
    UInteger width, height, depth;
};

inline Size sizeMake(UInteger width, UInteger height, UInteger depth) {
    Size size = {width, height, depth};
    return size;
}

//MARK: - Region
struct Region {
    Origin origin;
    Size size;
};

inline Region regionMake1D(UInteger x, UInteger width) {
    Region region;
    region.origin.x = x;
    region.origin.y = 0;
    region.origin.z = 0;
    region.size.width = width;
    region.size.height = 1;
    region.size.depth = 1;
    return region;
}

inline Region regionMake2D(UInteger x, UInteger y,
                           UInteger width, UInteger height) {
    Region region;
    region.origin.x = x;
    region.origin.y = y;
    region.origin.z = 0;
    region.size.width = width;
    region.size.height = height;
    region.size.depth = 1;
    return region;
}

inline Region regionMake3D(UInteger x, UInteger y, UInteger z,
                           UInteger width, UInteger height, UInteger depth) {
    Region region;
    region.origin.x = x;
    region.origin.y = y;
    region.origin.z = z;
    region.size.width = width;
    region.size.height = height;
    region.size.depth = depth;
    return region;
}

} // namespace MTL

#endif // cpp_mtl_Types_hpp
