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
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;

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

//MARK: -
typedef enum {
    Position = 0,
    Normal = 1,
    UV_0 = 2,
    Tangent = 3,
    Bitangent = 4,
    Color_0 = 5,
    Weights_0 = 6,
    Joints_0 = 7,
    UV_1 = 8,
    UV_2 = 9,
    UV_3 = 10,
    UV_4 = 11,
    UV_5 = 12,
    UV_6 = 13,
    UV_7 = 14,
} Attributes;

struct EnvMapLight {
    vector_float3 diffuse;
    float diffuseIntensity;
    float specularIntensity;
    int mipMapLevel;
    matrix_float4x4 transformMatrix;
};

struct PointLightData {
    vector_float3 color;
    vector_float3 position;
    float distance;
};

struct SpotLightData {
    vector_float3 color;
    vector_float3 position;
    vector_float3 direction;
    float distance;
    float angleCos;
    float penumbraCos;
};

struct DirectLightData {
    vector_float3 color;
    vector_float3 direction;
};

struct ShadowData {
    /**
     * Shadow bias.
     */
    float bias = 0.005;
    /**
     * Shadow intensity, the larger the value, the clearer and darker the shadow.
     */
    float intensity = 0.2;
    /**
     * Pixel range used for shadow PCF interpolation.
     */
    float radius = 1;
    /**
     * Light view projection matrix.(cascade)
     */
    matrix_float4x4 vp[4];
    /**
     * Light cascade depth.
     */
    float cascadeSplits[4];
};

struct CubeShadowData {
    /**
     * Shadow bias.
     */
    float bias = 0.005;
    /**
     * Shadow intensity, the larger the value, the clearer and darker the shadow.
     */
    float intensity = 0.2;
    /**
     * Pixel range used for shadow PCF interpolation.
     */
    float radius = 1;
    /**
     * Light view projection matrix.(cascade)
     */
    matrix_float4x4 vp[6];
    
    vector_float3 lightPos;
};

#endif /* ShaderTypes_h */

