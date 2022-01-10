/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for C++ Metal pixel format type wrapper
*/

#ifndef CPPMetalPixelFormat_h
#define CPPMetalPixelFormat_h

#include "CPPMetalImplementation.hpp"
#include "CPPMetalTypes.hpp"


namespace MTL
{


typedef enum PixelFormat
{
   PixelFormatInvalid = 0,

    /* Normal 8 bit formats */

   PixelFormatA8Unorm      = 1,

   PixelFormatR8Unorm                            = 10,

   PixelFormatR8Unorm_sRGB API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 11,

   PixelFormatR8Snorm      = 12,
   PixelFormatR8Uint       = 13,
   PixelFormatR8Sint       = 14,

    /* Normal 16 bit `formats` */

   PixelFormatR16Unorm     = 20,
   PixelFormatR16Snorm     = 22,
   PixelFormatR16Uint      = 23,
   PixelFormatR16Sint      = 24,
   PixelFormatR16Float     = 25,

   PixelFormatRG8Unorm                            = 30,
   PixelFormatRG8Unorm_sRGB API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 31,
   PixelFormatRG8Snorm                            = 32,
   PixelFormatRG8Uint                             = 33,
   PixelFormatRG8Sint                             = 34,

    /* Packed 16 bit formats */

   PixelFormatB5G6R5Unorm API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 40,
   PixelFormatA1BGR5Unorm API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 41,
   PixelFormatABGR4Unorm  API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 42,
   PixelFormatBGR5A1Unorm API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 43,

    /* Normal 32 bit formats */

   PixelFormatR32Uint  = 53,
   PixelFormatR32Sint  = 54,
   PixelFormatR32Float = 55,

   PixelFormatRG16Unorm  = 60,
   PixelFormatRG16Snorm  = 62,
   PixelFormatRG16Uint   = 63,
   PixelFormatRG16Sint   = 64,
   PixelFormatRG16Float  = 65,

   PixelFormatRGBA8Unorm      = 70,
   PixelFormatRGBA8Unorm_sRGB = 71,
   PixelFormatRGBA8Snorm      = 72,
   PixelFormatRGBA8Uint       = 73,
   PixelFormatRGBA8Sint       = 74,

   PixelFormatBGRA8Unorm      = 80,
   PixelFormatBGRA8Unorm_sRGB = 81,

    /* Packed 32 bit formats */

   PixelFormatRGB10A2Unorm = 90,
   PixelFormatRGB10A2Uint  = 91,

   PixelFormatRG11B10Float = 92,
   PixelFormatRGB9E5Float = 93,

   PixelFormatBGR10A2Unorm  API_AVAILABLE(macos(10.13), ios(11.0)) = 94,

   PixelFormatBGR10_XR      API_AVAILABLE(ios(10.0)) API_UNAVAILABLE(macos, macCatalyst) = 554,
   PixelFormatBGR10_XR_sRGB API_AVAILABLE(ios(10.0)) API_UNAVAILABLE(macos, macCatalyst) = 555,

    /* Normal 64 bit formats */

   PixelFormatRG32Uint  = 103,
   PixelFormatRG32Sint  = 104,
   PixelFormatRG32Float = 105,

   PixelFormatRGBA16Unorm  = 110,
   PixelFormatRGBA16Snorm  = 112,
   PixelFormatRGBA16Uint   = 113,
   PixelFormatRGBA16Sint   = 114,
   PixelFormatRGBA16Float  = 115,

   PixelFormatBGRA10_XR      API_AVAILABLE(ios(10.0)) API_UNAVAILABLE(macos, macCatalyst) = 552,
   PixelFormatBGRA10_XR_sRGB API_AVAILABLE(ios(10.0)) API_UNAVAILABLE(macos, macCatalyst) = 553,
    /* Normal 128 bit formats */

   PixelFormatRGBA32Uint  = 123,
   PixelFormatRGBA32Sint  = 124,
   PixelFormatRGBA32Float = 125,

    /* Compressed formats. */

    /* S3TC/DXT */

   PixelFormatBC1_RGBA              API_AVAILABLE(macos(10.11), macCatalyst(13.0)) API_UNAVAILABLE(ios) = 130,
   PixelFormatBC1_RGBA_sRGB         API_AVAILABLE(macos(10.11), macCatalyst(13.0)) API_UNAVAILABLE(ios) = 131,
   PixelFormatBC2_RGBA              API_AVAILABLE(macos(10.11), macCatalyst(13.0)) API_UNAVAILABLE(ios) = 132,
   PixelFormatBC2_RGBA_sRGB         API_AVAILABLE(macos(10.11), macCatalyst(13.0)) API_UNAVAILABLE(ios) = 133,
   PixelFormatBC3_RGBA              API_AVAILABLE(macos(10.11), macCatalyst(13.0)) API_UNAVAILABLE(ios) = 134,
   PixelFormatBC3_RGBA_sRGB         API_AVAILABLE(macos(10.11), macCatalyst(13.0)) API_UNAVAILABLE(ios) = 135,

    /* RGTC */
   PixelFormatBC4_RUnorm            API_AVAILABLE(macos(10.11), macCatalyst(13.0)) API_UNAVAILABLE(ios) = 140,
   PixelFormatBC4_RSnorm            API_AVAILABLE(macos(10.11), macCatalyst(13.0)) API_UNAVAILABLE(ios) = 141,
   PixelFormatBC5_RGUnorm           API_AVAILABLE(macos(10.11), macCatalyst(13.0)) API_UNAVAILABLE(ios) = 142,
   PixelFormatBC5_RGSnorm           API_AVAILABLE(macos(10.11), macCatalyst(13.0)) API_UNAVAILABLE(ios) = 143,

    /* BPTC */
   PixelFormatBC6H_RGBFloat         API_AVAILABLE(macos(10.11), macCatalyst(13.0)) API_UNAVAILABLE(ios) = 150,
   PixelFormatBC6H_RGBUfloat        API_AVAILABLE(macos(10.11), macCatalyst(13.0)) API_UNAVAILABLE(ios) = 151,
   PixelFormatBC7_RGBAUnorm         API_AVAILABLE(macos(10.11), macCatalyst(13.0)) API_UNAVAILABLE(ios) = 152,
   PixelFormatBC7_RGBAUnorm_sRGB    API_AVAILABLE(macos(10.11), macCatalyst(13.0)) API_UNAVAILABLE(ios) = 153,

    /* PVRTC */
   PixelFormatPVRTC_RGB_2BPP        API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 160,
   PixelFormatPVRTC_RGB_2BPP_sRGB   API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 161,
   PixelFormatPVRTC_RGB_4BPP        API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 162,
   PixelFormatPVRTC_RGB_4BPP_sRGB   API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 163,
   PixelFormatPVRTC_RGBA_2BPP       API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 164,
   PixelFormatPVRTC_RGBA_2BPP_sRGB  API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 165,
   PixelFormatPVRTC_RGBA_4BPP       API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 166,
   PixelFormatPVRTC_RGBA_4BPP_sRGB  API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 167,

    /* ETC2 */
   PixelFormatEAC_R11Unorm          API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 170,
   PixelFormatEAC_R11Snorm          API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 172,
   PixelFormatEAC_RG11Unorm         API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 174,
   PixelFormatEAC_RG11Snorm         API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 176,
   PixelFormatEAC_RGBA8             API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 178,
   PixelFormatEAC_RGBA8_sRGB        API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 179,

   PixelFormatETC2_RGB8             API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 180,
   PixelFormatETC2_RGB8_sRGB        API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 181,
   PixelFormatETC2_RGB8A1           API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 182,
   PixelFormatETC2_RGB8A1_sRGB      API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 183,

    /* ASTC */
   PixelFormatASTC_4x4_sRGB         API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 186,
   PixelFormatASTC_5x4_sRGB         API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 187,
   PixelFormatASTC_5x5_sRGB         API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 188,
   PixelFormatASTC_6x5_sRGB         API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 189,
   PixelFormatASTC_6x6_sRGB         API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 190,
   PixelFormatASTC_8x5_sRGB         API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 192,
   PixelFormatASTC_8x6_sRGB         API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 193,
   PixelFormatASTC_8x8_sRGB         API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 194,
   PixelFormatASTC_10x5_sRGB        API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 195,
   PixelFormatASTC_10x6_sRGB        API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 196,
   PixelFormatASTC_10x8_sRGB        API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 197,
   PixelFormatASTC_10x10_sRGB       API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 198,
   PixelFormatASTC_12x10_sRGB       API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 199,
   PixelFormatASTC_12x12_sRGB       API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 200,

   PixelFormatASTC_4x4_LDR          API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 204,
   PixelFormatASTC_5x4_LDR          API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 205,
   PixelFormatASTC_5x5_LDR          API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 206,
   PixelFormatASTC_6x5_LDR          API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 207,
   PixelFormatASTC_6x6_LDR          API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 208,
   PixelFormatASTC_8x5_LDR          API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 210,
   PixelFormatASTC_8x6_LDR          API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 211,
   PixelFormatASTC_8x8_LDR          API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 212,
   PixelFormatASTC_10x5_LDR         API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 213,
   PixelFormatASTC_10x6_LDR         API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 214,
   PixelFormatASTC_10x8_LDR         API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 215,
   PixelFormatASTC_10x10_LDR        API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 216,
   PixelFormatASTC_12x10_LDR        API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 217,
   PixelFormatASTC_12x12_LDR        API_AVAILABLE(ios(8.0)) API_UNAVAILABLE(macos, macCatalyst) = 218,

    PixelFormatGBGR422 = 240,

   PixelFormatBGRG422 = 241,

    /* Depth */

   PixelFormatDepth16Unorm = 250,
   PixelFormatDepth32Float = 252,

    /* Stencil */

   PixelFormatStencil8        = 253,

    /* Depth Stencil */
   PixelFormatDepth24Unorm_Stencil8  API_AVAILABLE(macos(10.11), macCatalyst(13.0)) API_UNAVAILABLE(ios) = 255,

   PixelFormatDepth32Float_Stencil8  API_AVAILABLE(macos(10.11), ios(9.0)) = 260,

   PixelFormatX32_Stencil8  API_AVAILABLE(macos(10.12), ios(10.0)) = 261,
   PixelFormatX24_Stencil8  API_AVAILABLE(macos(10.12), macCatalyst(13.0)) API_UNAVAILABLE(ios) = 262,

} PixelFormat API_AVAILABLE(macos(10.11), ios(8.0));


} // namespace MTL

#endif // CPPMetalPixelFormat_h

