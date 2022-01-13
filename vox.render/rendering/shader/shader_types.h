/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header containing types and enum constants shared between Metal shaders and C/ObjC source
*/
#ifndef ShaderTypes_h
#define ShaderTypes_h

#include "config.h"
#include <simd/simd.h>

#ifndef __METAL_VERSION__
/// 96-bit / 12 byte 3 component float vector type
typedef struct __attribute__ ((packed)) packed_float3 {
    float x;
    float y;
    float z;
} packed_float3;
#endif

// Buffer index values shared between shader and C code to ensure Metal shader buffer inputs match
//   Metal API buffer set calls
typedef enum BufferIndex
{
    BufferIndexMeshPositions     = 0,
    BufferIndexMeshGenerics      = 1,
    BufferIndexFrameData         = 2,
    BufferIndexLightsData        = 3,
    BufferIndexLightsPosition    = 4,
} BufferIndex;

// Attribute index values shared between shader and C code to ensure Metal shader vertex
//   attribute indices match the Metal API vertex descriptor attribute indices
typedef enum VertexAttributes
{
    VertexAttributePosition  = 0,
    VertexAttributeTexcoord  = 1,
    VertexAttributeNormal    = 2,
    VertexAttributeTangent   = 3,
    VertexAttributeBitangent = 4
} VertexAttributes;

// Texture index values shared between shader and C code to ensure Metal shader texture indices
//   match indices of Metal API texture set calls
typedef enum TextureIndex
{
	TextureIndexBaseColor = 0,
	TextureIndexSpecular  = 1,
	TextureIndexNormal    = 2,
    TextureIndexShadow    = 3,
    TextureIndexAlpha     = 4,

    NumMeshTextures = TextureIndexNormal + 1

} TextureIndex;

typedef enum RenderTargetIndex
{
    RenderTargetLighting  = 0,
    RenderTargetAlbedo    = 1,
    RenderTargetNormal    = 2,
    RenderTargetDepth     = 3
} RenderTargetIndex;

// Structures shared between shader and C code to ensure the layout of per frame data
//    accessed in Metal shaders matches the layout of fra data set in C code
//    Data constant across all threads, vertices, and fragments
struct FrameData
{
    // Per Frame Uniforms
    simd::float4x4 projection_matrix;
    simd::float4x4 projection_matrix_inverse;
    simd::float4x4 view_matrix;
    uint framebuffer_width;
    uint framebuffer_height;

    // Per Mesh Uniforms
    simd::float4x4 temple_modelview_matrix;
    simd::float4x4 temple_model_matrix;
    simd::float3x3 temple_normal_matrix;
    float shininess_factor;

    float fairy_size;
    float fairy_specular_intensity;

    simd::float4x4 sky_modelview_matrix;
    simd::float4x4 shadow_mvp_matrix;
    simd::float4x4 shadow_mvp_xform_matrix;

    simd::float4 sun_eye_direction;
    simd::float4 sun_color;
    float sun_specular_intensity;
};

// Per-light characteristics
struct PointLight
{
    simd::float3 light_color;
    float light_radius;
    float light_speed;
};

// Simple vertex used to render the "fairies"
struct SimpleVertex
{
           simd::float2 position;
};

struct ShadowVertex
{
    packed_float3 position;
};

#endif /* ShaderTypes_h */

