//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cpp_mtl_DataType_hpp
#define cpp_mtl_DataType_hpp

#include "cpp_mtl_implementation.h"
#include "cpp_mtl_types.h"

namespace MTL {

typedef enum DataType {
    DataTypeNone = 0,

    DataTypeStruct = 1,
    DataTypeArray  = 2,

    DataTypeFloat  = 3,
    DataTypeFloat2 = 4,
    DataTypeFloat3 = 5,
    DataTypeFloat4 = 6,

    DataTypeFloat2x2 = 7,
    DataTypeFloat2x3 = 8,
    DataTypeFloat2x4 = 9,

    DataTypeFloat3x2 = 10,
    DataTypeFloat3x3 = 11,
    DataTypeFloat3x4 = 12,

    DataTypeFloat4x2 = 13,
    DataTypeFloat4x3 = 14,
    DataTypeFloat4x4 = 15,

    DataTypeHalf  = 16,
    DataTypeHalf2 = 17,
    DataTypeHalf3 = 18,
    DataTypeHalf4 = 19,

    DataTypeHalf2x2 = 20,
    DataTypeHalf2x3 = 21,
    DataTypeHalf2x4 = 22,

    DataTypeHalf3x2 = 23,
    DataTypeHalf3x3 = 24,
    DataTypeHalf3x4 = 25,

    DataTypeHalf4x2 = 26,
    DataTypeHalf4x3 = 27,
    DataTypeHalf4x4 = 28,

    DataTypeInt  = 29,
    DataTypeInt2 = 30,
    DataTypeInt3 = 31,
    DataTypeInt4 = 32,

    DataTypeUInt  = 33,
    DataTypeUInt2 = 34,
    DataTypeUInt3 = 35,
    DataTypeUInt4 = 36,

    DataTypeShort  = 37,
    DataTypeShort2 = 38,
    DataTypeShort3 = 39,
    DataTypeShort4 = 40,

    DataTypeUShort = 41,
    DataTypeUShort2 = 42,
    DataTypeUShort3 = 43,
    DataTypeUShort4 = 44,

    DataTypeChar  = 45,
    DataTypeChar2 = 46,
    DataTypeChar3 = 47,
    DataTypeChar4 = 48,

    DataTypeUChar  = 49,
    DataTypeUChar2 = 50,
    DataTypeUChar3 = 51,
    DataTypeUChar4 = 52,

    DataTypeBool  = 53,
    DataTypeBool2 = 54,
    DataTypeBool3 = 55,
    DataTypeBool4 = 56,

    DataTypeTexture API_AVAILABLE(macos(10.13), ios(11.0)) = 58,
    DataTypeSampler API_AVAILABLE(macos(10.13), ios(11.0)) = 59,
    DataTypePointer API_AVAILABLE(macos(10.13), ios(11.0)) = 60,

    DataTypeR8Unorm         API_AVAILABLE(macos(11.0), macCatalyst(14.0), ios(11.0), tvos(14.5)) = 62,
    DataTypeR8Snorm         API_AVAILABLE(macos(11.0), macCatalyst(14.0), ios(11.0), tvos(14.5)) = 63,
    DataTypeR16Unorm        API_AVAILABLE(macos(11.0), macCatalyst(14.0), ios(11.0), tvos(14.5)) = 64,
    DataTypeR16Snorm        API_AVAILABLE(macos(11.0), macCatalyst(14.0), ios(11.0), tvos(14.5)) = 65,
    DataTypeRG8Unorm        API_AVAILABLE(macos(11.0), macCatalyst(14.0), ios(11.0), tvos(14.5)) = 66,
    DataTypeRG8Snorm        API_AVAILABLE(macos(11.0), macCatalyst(14.0), ios(11.0), tvos(14.5)) = 67,
    DataTypeRG16Unorm       API_AVAILABLE(macos(11.0), macCatalyst(14.0), ios(11.0), tvos(14.5)) = 68,
    DataTypeRG16Snorm       API_AVAILABLE(macos(11.0), macCatalyst(14.0), ios(11.0), tvos(14.5)) = 69,
    DataTypeRGBA8Unorm      API_AVAILABLE(macos(11.0), macCatalyst(14.0), ios(11.0), tvos(14.5)) = 70,
    DataTypeRGBA8Unorm_sRGB API_AVAILABLE(macos(11.0), macCatalyst(14.0), ios(11.0), tvos(14.5)) = 71,
    DataTypeRGBA8Snorm      API_AVAILABLE(macos(11.0), macCatalyst(14.0), ios(11.0), tvos(14.5)) = 72,
    DataTypeRGBA16Unorm     API_AVAILABLE(macos(11.0), macCatalyst(14.0), ios(11.0), tvos(14.5)) = 73,
    DataTypeRGBA16Snorm     API_AVAILABLE(macos(11.0), macCatalyst(14.0), ios(11.0), tvos(14.5)) = 74,
    DataTypeRGB10A2Unorm    API_AVAILABLE(macos(11.0), macCatalyst(14.0), ios(11.0), tvos(14.5)) = 75,
    DataTypeRG11B10Float    API_AVAILABLE(macos(11.0), macCatalyst(14.0), ios(11.0), tvos(14.5)) = 76,
    DataTypeRGB9E5Float     API_AVAILABLE(macos(11.0), macCatalyst(14.0), ios(11.0), tvos(14.5)) = 77,
    DataTypeRenderPipeline  API_AVAILABLE(macos(10.14), ios(13.0)) = 78,
    DataTypeComputePipeline API_AVAILABLE(macos(11.0), macCatalyst(14.0), ios(13.0)) = 79,
    DataTypeIndirectCommandBuffer   API_AVAILABLE(macos(10.14), ios(12.0)) = 80,
    DataTypeLong  API_AVAILABLE(macos(12.0), ios(14.0)) = 81,
    DataTypeLong2 API_AVAILABLE(macos(12.0), ios(14.0)) = 82,
    DataTypeLong3 API_AVAILABLE(macos(12.0), ios(14.0)) = 83,
    DataTypeLong4 API_AVAILABLE(macos(12.0), ios(14.0)) = 84,

    DataTypeULong  API_AVAILABLE(macos(12.0), ios(14.0)) = 85,
    DataTypeULong2 API_AVAILABLE(macos(12.0), ios(14.0)) = 86,
    DataTypeULong3 API_AVAILABLE(macos(12.0), ios(14.0)) = 87,
    DataTypeULong4 API_AVAILABLE(macos(12.0), ios(14.0)) = 88,
    DataTypeVisibleFunctionTable API_AVAILABLE(macos(11.0), ios(14.0)) = 115,
    DataTypeIntersectionFunctionTable API_AVAILABLE(macos(11.0), ios(14.0)) = 116,
    DataTypePrimitiveAccelerationStructure API_AVAILABLE(macos(11.0), ios(14.0)) = 117,
    DataTypeInstanceAccelerationStructure API_AVAILABLE(macos(11.0), ios(14.0)) = 118,
} DataType API_AVAILABLE(macos(10.11), ios(8.0));


} // namespace MTL

#endif /* cpp_mtl_DataType_hpp */
