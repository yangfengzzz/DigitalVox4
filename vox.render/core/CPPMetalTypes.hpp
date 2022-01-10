/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for C++ Metal generic type wrappers
*/

#ifndef CPPMetalTypes_hpp
#define CPPMetalTypes_hpp

#include <cstdint>
#include <climits>

namespace MTL
{


typedef unsigned long UInteger;
static const UInteger UIntegerMax = ULONG_MAX;

struct Range
{
    UInteger location;
    UInteger length;
};

struct Origin
{
    UInteger x, y, z;
};

inline Origin OriginMake(UInteger x, UInteger y, UInteger z)
{
    Origin origin = {x, y, z};
    return origin;
}

struct Size
{
    UInteger width, height, depth;
};

inline Size SizeMake(UInteger width, UInteger height, UInteger depth)
{
    Size size = {width, height, depth};
    return size;
}

struct Region
{
    Origin origin;
    Size   size;
};


inline Region RegionMake1D(UInteger x, UInteger width)
{
    Region region;
    region.origin.x = x; region.origin.y = 0; region.origin.z = 0;
    region.size.width = width; region.size.height = 1; region.size.depth = 1;
    return region;
}

inline Region RegionMake2D(UInteger x, UInteger y, UInteger width, UInteger height)
{
    Region region;
    region.origin.x = x; region.origin.y = y; region.origin.z = 0;
    region.size.width = width; region.size.height = height; region.size.depth = 1;
    return region;
}

inline Region RegionMake3D(UInteger x, UInteger y, UInteger z, UInteger width, UInteger height, UInteger depth)
{
    Region region;
    region.origin.x = x; region.origin.y = y; region.origin.z = z;
    region.size.width = width; region.size.height = height; region.size.depth = depth;
    return region;
}


} // namespace MTL

#endif // CPPMetalTypes_hpp
