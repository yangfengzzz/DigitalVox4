/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Metal shaders used to render shadow maps
*/

// Include header shared between this Metal shader code and C code executing Metal API commands
#include "shader_types.h"

struct ShadowOutput
{
    float4 position [[position]];
};

vertex ShadowOutput shadow_vertex(const device ShadowVertex *positions [[ buffer(BufferIndexMeshPositions) ]],
                                  constant     FrameData    &frameData [[ buffer(BufferIndexFrameData) ]],
                                  uint                             vid [[ vertex_id ]])
{
    ShadowOutput out;

    // Add vertex pos to fairy position and project to clip-space
    out.position = frameData.shadow_mvp_matrix * float4(positions[vid].position, 1.0);

    return out;
}
