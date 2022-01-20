//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "primitive_mesh.h"
#include "rendering/math_utilities.h"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

using namespace MTL;

namespace vox {
MeshPtr PrimitiveMesh::createSphere(MTL::Device *device,
                                    float radius,
                                    size_t segments,
                                    bool noLongerAccessible) {
    auto mesh = std::make_shared<ModelMesh>(device);
    segments = std::max(size_t(2), segments);
    
    const auto count = segments + 1;
    const auto vertexCount = count * count;
    const auto rectangleCount = segments * segments;
    auto indices = std::vector<uint32_t>(rectangleCount * 6);
    const auto thetaRange = M_PI;
    const auto alphaRange = thetaRange * 2;
    const auto countReciprocal = 1.0 / count;
    const auto segmentsReciprocal = 1.0 / segments;
    
    auto positions = std::vector<Vector3F>(vertexCount);
    auto normals = std::vector<Vector3F>(vertexCount);
    auto uvs = std::vector<Vector2F>(vertexCount);
    
    for (size_t i = 0; i < vertexCount; ++i) {
        const auto x = i % count;
        const auto y = size_t(float(i) * countReciprocal) | 0;
        const auto u = x * segmentsReciprocal;
        const auto v = y * segmentsReciprocal;
        const auto alphaDelta = u * alphaRange;
        const auto thetaDelta = v * thetaRange;
        const auto sinTheta = std::sin(thetaDelta);
        
        const auto posX = -radius * std::cos(alphaDelta) * sinTheta;
        const auto posY = radius * std::cos(thetaDelta);
        const auto posZ = radius * std::sin(alphaDelta) * sinTheta;
        
        // Position
        positions[i] = Vector3F(posX, posY, posZ);
        // Normal
        normals[i] = Vector3F(posX, posY, posZ);
        // Texcoord
        uvs[i] = Vector2F(u, v);
    }
    
    size_t offset = 0;
    for (size_t i = 0; i < rectangleCount; ++i) {
        const auto x = i % segments;
        const auto y = size_t(float(i) * segmentsReciprocal) | 0;
        
        const auto a = y * count + x;
        const auto b = a + 1;
        const auto c = a + count;
        const auto d = c + 1;
        
        indices[offset++] = static_cast<uint32_t>(b);
        indices[offset++] = static_cast<uint32_t>(a);
        indices[offset++] = static_cast<uint32_t>(d);
        indices[offset++] = static_cast<uint32_t>(a);
        indices[offset++] = static_cast<uint32_t>(c);
        indices[offset++] = static_cast<uint32_t>(d);
    }
    
    auto &bounds = mesh->bounds;
    bounds.lowerCorner = Point3F(-radius, -radius, -radius);
    bounds.upperCorner = Point3F(radius, radius, radius);
    
    PrimitiveMesh::_initialize(device, mesh, positions, normals, uvs, indices, noLongerAccessible);
    return mesh;
}

MeshPtr PrimitiveMesh::createCuboid(MTL::Device *device,
                                    float width,
                                    float height,
                                    float depth,
                                    bool noLongerAccessible) {
    auto mesh = std::make_shared<ModelMesh>(device);
    
    const auto halfWidth = width / 2;
    const auto halfHeight = height / 2;
    const auto halfDepth = depth / 2;
    
    auto positions = std::vector<Vector3F>(24);
    auto normals = std::vector<Vector3F>(24);
    auto uvs = std::vector<Vector2F>(24);
    
    // Up
    positions[0] = Vector3F(-halfWidth, halfHeight, -halfDepth);
    positions[1] = Vector3F(halfWidth, halfHeight, -halfDepth);
    positions[2] = Vector3F(halfWidth, halfHeight, halfDepth);
    positions[3] = Vector3F(-halfWidth, halfHeight, halfDepth);
    normals[0] = Vector3F(0, 1, 0);
    normals[1] = Vector3F(0, 1, 0);
    normals[2] = Vector3F(0, 1, 0);
    normals[3] = Vector3F(0, 1, 0);
    uvs[0] = Vector2F(0, 0);
    uvs[1] = Vector2F(1, 0);
    uvs[2] = Vector2F(1, 1);
    uvs[3] = Vector2F(0, 1);
    // Down
    positions[4] = Vector3F(-halfWidth, -halfHeight, -halfDepth);
    positions[5] = Vector3F(halfWidth, -halfHeight, -halfDepth);
    positions[6] = Vector3F(halfWidth, -halfHeight, halfDepth);
    positions[7] = Vector3F(-halfWidth, -halfHeight, halfDepth);
    normals[4] = Vector3F(0, -1, 0);
    normals[5] = Vector3F(0, -1, 0);
    normals[6] = Vector3F(0, -1, 0);
    normals[7] = Vector3F(0, -1, 0);
    uvs[4] = Vector2F(0, 1);
    uvs[5] = Vector2F(1, 1);
    uvs[6] = Vector2F(1, 0);
    uvs[7] = Vector2F(0, 0);
    // Left
    positions[8] = Vector3F(-halfWidth, halfHeight, -halfDepth);
    positions[9] = Vector3F(-halfWidth, halfHeight, halfDepth);
    positions[10] = Vector3F(-halfWidth, -halfHeight, halfDepth);
    positions[11] = Vector3F(-halfWidth, -halfHeight, -halfDepth);
    normals[8] = Vector3F(-1, 0, 0);
    normals[9] = Vector3F(-1, 0, 0);
    normals[10] = Vector3F(-1, 0, 0);
    normals[11] = Vector3F(-1, 0, 0);
    uvs[8] = Vector2F(0, 0);
    uvs[9] = Vector2F(1, 0);
    uvs[10] = Vector2F(1, 1);
    uvs[11] = Vector2F(0, 1);
    // Right
    positions[12] = Vector3F(halfWidth, halfHeight, -halfDepth);
    positions[13] = Vector3F(halfWidth, halfHeight, halfDepth);
    positions[14] = Vector3F(halfWidth, -halfHeight, halfDepth);
    positions[15] = Vector3F(halfWidth, -halfHeight, -halfDepth);
    normals[12] = Vector3F(1, 0, 0);
    normals[13] = Vector3F(1, 0, 0);
    normals[14] = Vector3F(1, 0, 0);
    normals[15] = Vector3F(1, 0, 0);
    uvs[12] = Vector2F(1, 0);
    uvs[13] = Vector2F(0, 0);
    uvs[14] = Vector2F(0, 1);
    uvs[15] = Vector2F(1, 1);
    // Front
    positions[16] = Vector3F(-halfWidth, halfHeight, halfDepth);
    positions[17] = Vector3F(halfWidth, halfHeight, halfDepth);
    positions[18] = Vector3F(halfWidth, -halfHeight, halfDepth);
    positions[19] = Vector3F(-halfWidth, -halfHeight, halfDepth);
    normals[16] = Vector3F(0, 0, 1);
    normals[17] = Vector3F(0, 0, 1);
    normals[18] = Vector3F(0, 0, 1);
    normals[19] = Vector3F(0, 0, 1);
    uvs[16] = Vector2F(0, 0);
    uvs[17] = Vector2F(1, 0);
    uvs[18] = Vector2F(1, 1);
    uvs[19] = Vector2F(0, 1);
    // Back
    positions[20] = Vector3F(-halfWidth, halfHeight, -halfDepth);
    positions[21] = Vector3F(halfWidth, halfHeight, -halfDepth);
    positions[22] = Vector3F(halfWidth, -halfHeight, -halfDepth);
    positions[23] = Vector3F(-halfWidth, -halfHeight, -halfDepth);
    normals[20] = Vector3F(0, 0, -1);
    normals[21] = Vector3F(0, 0, -1);
    normals[22] = Vector3F(0, 0, -1);
    normals[23] = Vector3F(0, 0, -1);
    uvs[20] = Vector2F(1, 0);
    uvs[21] = Vector2F(0, 0);
    uvs[22] = Vector2F(0, 1);
    uvs[23] = Vector2F(1, 1);
    
    auto indices = std::vector<uint32_t>(36);
    
    // prettier-ignore
    // Up
    indices[0] = 0;
    indices[1] = 2;
    indices[2] = 1;
    indices[3] = 2;
    indices[4] = 0;
    indices[5] = 3;
    // Down
    indices[6] = 4;
    indices[7] = 6;
    indices[8] = 7;
    indices[9] = 6;
    indices[10] = 4;
    indices[11] = 5;
    // Left
    indices[12] = 8;
    indices[13] = 10;
    indices[14] = 9;
    indices[15] = 10;
    indices[16] = 8;
    indices[17] = 11;
    // Right
    indices[18] = 12;
    indices[19] = 14;
    indices[20] = 15;
    indices[21] = 14;
    indices[22] = 12;
    indices[23] = 13;
    // Front
    indices[24] = 16;
    indices[25] = 18;
    indices[26] = 17;
    indices[27] = 18;
    indices[28] = 16;
    indices[29] = 19;
    // Back
    indices[30] = 20;
    indices[31] = 22;
    indices[32] = 23;
    indices[33] = 22;
    indices[34] = 20;
    indices[35] = 21;
    
    auto &bounds = mesh->bounds;
    bounds.lowerCorner = Point3F(-halfWidth, -halfHeight, -halfDepth);
    bounds.upperCorner = Point3F(halfWidth, halfHeight, halfDepth);
    
    PrimitiveMesh::_initialize(device, mesh, positions, normals, uvs, indices, noLongerAccessible);
    return mesh;
    
}

MeshPtr PrimitiveMesh::createPlane(Device *device,
                                   float width,
                                   float height,
                                   size_t horizontalSegments,
                                   size_t verticalSegments,
                                   bool noLongerAccessible) {
    auto mesh = std::make_shared<ModelMesh>(device);
    horizontalSegments = std::max(size_t(1), horizontalSegments);
    verticalSegments = std::max(size_t(1), verticalSegments);
    
    const auto horizontalCount = horizontalSegments + 1;
    const auto verticalCount = verticalSegments + 1;
    const auto halfWidth = width / 2;
    const auto halfHeight = height / 2;
    const auto gridWidth = width / horizontalSegments;
    const auto gridHeight = height / verticalSegments;
    const auto vertexCount = horizontalCount * verticalCount;
    const auto rectangleCount = verticalSegments * horizontalSegments;
    auto indices = std::vector<uint32_t>(rectangleCount * 6);
    const auto horizontalCountReciprocal = 1.0 / horizontalCount;
    const auto horizontalSegmentsReciprocal = 1.0 / horizontalSegments;
    const auto verticalSegmentsReciprocal = 1.0 / verticalSegments;
    
    auto positions = std::vector<Vector3F>(vertexCount);
    auto normals = std::vector<Vector3F>(vertexCount);
    auto uvs = std::vector<Vector2F>(vertexCount);
    
    for (size_t i = 0; i < vertexCount; ++i) {
        const auto x = i % horizontalCount;
        const auto z = size_t(float(i) * horizontalCountReciprocal) | 0;
        
        // Position
        positions[i] = Vector3F(x * gridWidth - halfWidth, 0, z * gridHeight - halfHeight);
        // Normal
        normals[i] = Vector3F(0, 1, 0);
        // Texcoord
        uvs[i] = Vector2F(x * horizontalSegmentsReciprocal, z * verticalSegmentsReciprocal);
    }
    
    size_t offset = 0;
    for (size_t i = 0; i < rectangleCount; ++i) {
        const auto x = i % horizontalSegments;
        const auto y = size_t(float(i) * horizontalSegmentsReciprocal) | 0;
        
        const auto a = y * horizontalCount + x;
        const auto b = a + 1;
        const auto c = a + horizontalCount;
        const auto d = c + 1;
        
        indices[offset++] = static_cast<uint32_t>(a);
        indices[offset++] = static_cast<uint32_t>(c);
        indices[offset++] = static_cast<uint32_t>(b);
        indices[offset++] = static_cast<uint32_t>(c);
        indices[offset++] = static_cast<uint32_t>(d);
        indices[offset++] = static_cast<uint32_t>(b);
    }
    
    auto &bounds = mesh->bounds;
    bounds.lowerCorner = Point3F(-halfWidth, 0, -halfHeight);
    bounds.upperCorner = Point3F(halfWidth, 0, halfHeight);
    
    PrimitiveMesh::_initialize(device, mesh, positions, normals, uvs, indices, noLongerAccessible);
    return mesh;
}

MeshPtr PrimitiveMesh::createCylinder(Device *device,
                                      float radiusTop,
                                      float radiusBottom,
                                      float height,
                                      size_t radialSegments,
                                      size_t heightSegments,
                                      bool noLongerAccessible) {
    auto mesh = std::make_shared<ModelMesh>(device);
    
    const auto radialCount = radialSegments + 1;
    const auto verticalCount = heightSegments + 1;
    const auto halfHeight = height * 0.5;
    const float unitHeight = height / heightSegments;
    const auto torsoVertexCount = radialCount * verticalCount;
    const auto torsoRectangleCount = radialSegments * heightSegments;
    const auto capTriangleCount = radialSegments * 2;
    const auto totalVertexCount = torsoVertexCount + 2 + capTriangleCount;
    auto indices = std::vector<uint32_t>(torsoRectangleCount * 6 + capTriangleCount * 3);
    const float radialCountReciprocal = 1.0 / radialCount;
    const float radialSegmentsReciprocal = 1.0 / radialSegments;
    const float heightSegmentsReciprocal = 1.0 / heightSegments;
    
    auto positions = std::vector<Vector3F>(totalVertexCount);
    auto normals = std::vector<Vector3F>(totalVertexCount);
    auto uvs = std::vector<Vector2F>(totalVertexCount);
    
    size_t indicesOffset = 0;
    
    // Create torso
    const auto thetaStart = M_PI;
    const auto thetaRange = M_PI * 2;
    const auto radiusDiff = radiusBottom - radiusTop;
    const auto slope = radiusDiff / height;
    const float radiusSlope = radiusDiff / heightSegments;
    
    for (size_t i = 0; i < torsoVertexCount; ++i) {
        const auto x = i % radialCount;
        const auto y = size_t(float(i) * radialCountReciprocal) | 0;
        const auto u = x * radialSegmentsReciprocal;
        const auto v = y * heightSegmentsReciprocal;
        const auto theta = thetaStart + u * thetaRange;
        const auto sinTheta = std::sin(theta);
        const auto cosTheta = std::cos(theta);
        const auto radius = radiusBottom - y * radiusSlope;
        
        const auto posX = radius * sinTheta;
        const auto posY = y * unitHeight - halfHeight;
        const auto posZ = radius * cosTheta;
        
        // Position
        positions[i] = Vector3F(posX, posY, posZ);
        // Normal
        normals[i] = Vector3F(sinTheta, slope, cosTheta);
        // Texcoord
        uvs[i] = Vector2F(u, 1 - v);
    }
    
    for (size_t i = 0; i < torsoRectangleCount; ++i) {
        const auto x = i % radialSegments;
        const auto y = size_t(float(i) * radialSegmentsReciprocal) | 0;
        
        const auto a = y * radialCount + x;
        const auto b = a + 1;
        const auto c = a + radialCount;
        const auto d = c + 1;
        
        indices[indicesOffset++] = static_cast<uint32_t>(b);
        indices[indicesOffset++] = static_cast<uint32_t>(c);
        indices[indicesOffset++] = static_cast<uint32_t>(a);
        indices[indicesOffset++] = static_cast<uint32_t>(b);
        indices[indicesOffset++] = static_cast<uint32_t>(d);
        indices[indicesOffset++] = static_cast<uint32_t>(c);
    }
    
    // Bottom position
    positions[torsoVertexCount] = Vector3F(0, -halfHeight, 0);
    // Bottom normal
    normals[torsoVertexCount] = Vector3F(0, -1, 0);
    // Bottom texcoord
    uvs[torsoVertexCount] = Vector2F(0.5, 0.5);
    
    // Top position
    positions[torsoVertexCount + 1] = Vector3F(0, halfHeight, 0);
    // Top normal
    normals[torsoVertexCount + 1] = Vector3F(0, 1, 0);
    // Top texcoord
    uvs[torsoVertexCount + 1] = Vector2F(0.5, 0.5);
    
    // Add cap vertices
    auto offset = torsoVertexCount + 2;
    
    const auto diameterTopReciprocal = 1.0 / (radiusTop * 2);
    const auto diameterBottomReciprocal = 1.0 / (radiusBottom * 2);
    const auto positionStride = radialCount * heightSegments;
    for (size_t i = 0; i < radialSegments; ++i) {
        const auto curPosBottom = positions[i];
        auto curPosX = curPosBottom.x;
        auto curPosZ = curPosBottom.z;
        
        // Bottom position
        positions[offset] = Vector3F(curPosX, -halfHeight, curPosZ);
        // Bottom normal
        normals[offset] = Vector3F(0, -1, 0);
        // Bottom texcoord
        uvs[offset++] = Vector2F(curPosX * diameterBottomReciprocal + 0.5, 0.5 - curPosZ * diameterBottomReciprocal);
        
        const auto &curPosTop = positions[i + positionStride];
        curPosX = curPosTop.x;
        curPosZ = curPosTop.z;
        
        // Top position
        positions[offset] = Vector3F(curPosX, halfHeight, curPosZ);
        // Top normal
        normals[offset] = Vector3F(0, 1, 0);
        // Top texcoord
        uvs[offset++] = Vector2F(curPosX * diameterTopReciprocal + 0.5, curPosZ * diameterTopReciprocal + 0.5);
    }
    
    // Add cap indices
    const auto topCapIndex = torsoVertexCount + 1;
    const auto bottomIndiceIndex = torsoVertexCount + 2;
    const auto topIndiceIndex = bottomIndiceIndex + 1;
    for (size_t i = 0; i < radialSegments; ++i) {
        const auto firstStride = i * 2;
        const auto secondStride = i == radialSegments - 1 ? 0 : firstStride + 2;
        
        // Bottom
        indices[indicesOffset++] = static_cast<uint32_t>(torsoVertexCount);
        indices[indicesOffset++] = static_cast<uint32_t>(bottomIndiceIndex + secondStride);
        indices[indicesOffset++] = static_cast<uint32_t>(bottomIndiceIndex + firstStride);
        
        // Top
        indices[indicesOffset++] = static_cast<uint32_t>(topCapIndex);
        indices[indicesOffset++] = static_cast<uint32_t>(topIndiceIndex + firstStride);
        indices[indicesOffset++] = static_cast<uint32_t>(topIndiceIndex + secondStride);
    }
    
    auto &bounds = mesh->bounds;
    const auto radiusMax = std::max(radiusTop, radiusBottom);
    bounds.lowerCorner = Point3F(-radiusMax, -halfHeight, -radiusMax);
    bounds.upperCorner = Point3F(radiusMax, halfHeight, radiusMax);
    
    PrimitiveMesh::_initialize(device, mesh, positions, normals, uvs, indices, noLongerAccessible);
    return mesh;
}

MeshPtr PrimitiveMesh::createTorus(Device *device,
                                   float radius,
                                   float tubeRadius,
                                   size_t radialSegments,
                                   size_t tubularSegments,
                                   float arc,
                                   bool noLongerAccessible) {
    auto mesh = std::make_shared<ModelMesh>(device);
    
    const auto vertexCount = (radialSegments + 1) * (tubularSegments + 1);
    const auto rectangleCount = radialSegments * tubularSegments;
    auto indices = std::vector<uint32_t>(rectangleCount * 6);
    
    auto positions = std::vector<Vector3F>(vertexCount);
    auto normals = std::vector<Vector3F>(vertexCount);
    auto uvs = std::vector<Vector2F>(vertexCount);
    
    arc = (arc / 180) * M_PI;
    
    size_t offset = 0;
    for (size_t i = 0; i <= radialSegments; i++) {
        for (size_t j = 0; j <= tubularSegments; j++) {
            const auto u = (j / tubularSegments) * arc;
            const auto v = (i / radialSegments) * M_PI * 2;
            const auto cosV = std::cos(v);
            const auto sinV = std::sin(v);
            const auto cosU = std::cos(u);
            const auto sinU = std::sin(u);
            
            const auto position = Vector3F((radius + tubeRadius * cosV) * cosU,
                                           (radius + tubeRadius * cosV) * sinU,
                                           tubeRadius * sinV);
            positions[offset] = position;
            
            const auto centerX = radius * cosU;
            const auto centerY = radius * sinU;
            normals[offset] = Vector3F(position.x - centerX, position.y - centerY, position.z).normalized();
            
            uvs[offset++] = Vector2F(j / tubularSegments, i / radialSegments);
        }
    }
    
    offset = 0;
    for (size_t i = 1; i <= radialSegments; i++) {
        for (size_t j = 1; j <= tubularSegments; j++) {
            const auto a = (tubularSegments + 1) * i + j - 1;
            const auto b = (tubularSegments + 1) * (i - 1) + j - 1;
            const auto c = (tubularSegments + 1) * (i - 1) + j;
            const auto d = (tubularSegments + 1) * i + j;
            
            indices[offset++] = static_cast<uint32_t>(a);
            indices[offset++] = static_cast<uint32_t>(b);
            indices[offset++] = static_cast<uint32_t>(d);
            
            indices[offset++] = static_cast<uint32_t>(b);
            indices[offset++] = static_cast<uint32_t>(c);
            indices[offset++] = static_cast<uint32_t>(d);
        }
    }
    
    auto &bounds = mesh->bounds;
    const auto outerRadius = radius + tubeRadius;
    bounds.lowerCorner = Point3F(-outerRadius, -outerRadius, -tubeRadius);
    bounds.upperCorner = Point3F(outerRadius, outerRadius, tubeRadius);
    
    PrimitiveMesh::_initialize(device, mesh, positions, normals, uvs, indices, noLongerAccessible);
    return mesh;
}

MeshPtr PrimitiveMesh::createCone(Device *device,
                                  float radius,
                                  float height,
                                  size_t radialSegments,
                                  size_t heightSegments,
                                  bool noLongerAccessible) {
    auto mesh = std::make_shared<ModelMesh>(device);
    
    const auto radialCount = radialSegments + 1;
    const auto verticalCount = heightSegments + 1;
    const auto halfHeight = height * 0.5;
    const auto unitHeight = height / heightSegments;
    const auto torsoVertexCount = radialCount * verticalCount;
    const auto torsoRectangleCount = radialSegments * heightSegments;
    const auto totalVertexCount = torsoVertexCount + 1 + radialSegments;
    auto indices = std::vector<uint32_t>(torsoRectangleCount * 6 + radialSegments * 3);
    const auto radialCountReciprocal = 1.0 / radialCount;
    const auto radialSegmentsReciprocal = 1.0 / radialSegments;
    const auto heightSegmentsReciprocal = 1.0 / heightSegments;
    
    auto positions = std::vector<Vector3F>(totalVertexCount);
    auto normals = std::vector<Vector3F>(totalVertexCount);
    auto uvs = std::vector<Vector2F>(totalVertexCount);
    
    size_t indicesOffset = 0;
    
    // Create torso
    const auto thetaStart = M_PI;
    const auto thetaRange = M_PI * 2;
    const auto slope = radius / height;
    
    for (size_t i = 0; i < torsoVertexCount; ++i) {
        const auto x = i % radialCount;
        const auto y = size_t(float(i) * radialCountReciprocal) | 0;
        const auto u = x * radialSegmentsReciprocal;
        const auto v = y * heightSegmentsReciprocal;
        const auto theta = thetaStart + u * thetaRange;
        const auto sinTheta = std::sin(theta);
        const auto cosTheta = std::cos(theta);
        const auto curRadius = radius - y * radius;
        
        const auto posX = curRadius * sinTheta;
        const auto posY = y * unitHeight - halfHeight;
        const auto posZ = curRadius * cosTheta;
        
        // Position
        positions[i] = Vector3F(posX, posY, posZ);
        // Normal
        normals[i] = Vector3F(sinTheta, slope, cosTheta);
        // Texcoord
        uvs[i] = Vector2F(u, 1 - v);
    }
    
    for (size_t i = 0; i < torsoRectangleCount; ++i) {
        const auto x = i % radialSegments;
        const auto y = size_t(float(i) * radialSegmentsReciprocal) | 0;
        
        const auto a = y * radialCount + x;
        const auto b = a + 1;
        const auto c = a + radialCount;
        const auto d = c + 1;
        
        indices[indicesOffset++] = static_cast<uint32_t>(b);
        indices[indicesOffset++] = static_cast<uint32_t>(c);
        indices[indicesOffset++] = static_cast<uint32_t>(a);
        indices[indicesOffset++] = static_cast<uint32_t>(b);
        indices[indicesOffset++] = static_cast<uint32_t>(d);
        indices[indicesOffset++] = static_cast<uint32_t>(c);
    }
    
    // Bottom position
    positions[torsoVertexCount] = Vector3F(0, -halfHeight, 0);
    // Bottom normal
    normals[torsoVertexCount] = Vector3F(0, -1, 0);
    // Bottom texcoord
    uvs[torsoVertexCount] = Vector2F(0.5, 0.5);
    
    // Add bottom cap vertices
    size_t offset = torsoVertexCount + 1;
    const auto diameterBottomReciprocal = 1.0 / (radius * 2);
    for (size_t i = 0; i < radialSegments; ++i) {
        const auto &curPos = positions[i];
        const auto curPosX = curPos.x;
        const auto curPosZ = curPos.z;
        
        // Bottom position
        positions[offset] = Vector3F(curPosX, -halfHeight, curPosZ);
        // Bottom normal
        normals[offset] = Vector3F(0, -1, 0);
        // Bottom texcoord
        uvs[offset++] = Vector2F(curPosX * diameterBottomReciprocal + 0.5, 0.5 - curPosZ * diameterBottomReciprocal);
    }
    
    const auto bottomIndiceIndex = torsoVertexCount + 1;
    for (size_t i = 0; i < radialSegments; ++i) {
        const auto firstStride = i;
        const auto secondStride = i == radialSegments - 1 ? 0 : firstStride + 1;
        
        // Bottom
        indices[indicesOffset++] = static_cast<uint32_t>(torsoVertexCount);
        indices[indicesOffset++] = static_cast<uint32_t>(bottomIndiceIndex + secondStride);
        indices[indicesOffset++] = static_cast<uint32_t>(bottomIndiceIndex + firstStride);
    }
    
    auto &bounds = mesh->bounds;
    bounds.lowerCorner = Point3F(-radius, -halfHeight, -radius);
    bounds.upperCorner = Point3F(radius, halfHeight, radius);
    
    PrimitiveMesh::_initialize(device, mesh, positions, normals, uvs, indices, noLongerAccessible);
    return mesh;
}

MeshPtr PrimitiveMesh::createCapsule(Device *device,
                                          float radius,
                                          float height,
                                          size_t radialSegments,
                                          size_t heightSegments,
                                          bool noLongerAccessible) {
    auto mesh = std::make_shared<ModelMesh>(device);
    
    radialSegments = std::max(size_t(2), radialSegments);
    
    const auto radialCount = radialSegments + 1;
    const auto verticalCount = heightSegments + 1;
    const auto halfHeight = height * 0.5;
    const auto unitHeight = height / heightSegments;
    const auto torsoVertexCount = radialCount * verticalCount;
    const auto torsoRectangleCount = radialSegments * heightSegments;
    
    const auto capVertexCount = radialCount * radialCount;
    const auto capRectangleCount = radialSegments * radialSegments;
    
    const auto totalVertexCount = torsoVertexCount + 2 * capVertexCount;
    auto indices = std::vector<uint32_t>((torsoRectangleCount + 2 * capRectangleCount) * 6);
    
    const auto radialCountReciprocal = 1.0 / radialCount;
    const auto radialSegmentsReciprocal = 1.0 / radialSegments;
    const auto heightSegmentsReciprocal = 1.0 / heightSegments;
    
    const auto halfPI = M_PI / 2;
    const auto doublePI = M_PI * 2;
    
    auto positions = std::vector<Vector3F>(totalVertexCount);
    auto normals = std::vector<Vector3F>(totalVertexCount);
    auto uvs = std::vector<Vector2F>(totalVertexCount);
    
    size_t indicesOffset = 0;
    
    // create torso
    for (size_t i = 0; i < torsoVertexCount; ++i) {
        const auto x = i % radialCount;
        const auto y = size_t(float(i) * radialCountReciprocal) | 0;
        const auto u = x * radialSegmentsReciprocal;
        const auto v = y * heightSegmentsReciprocal;
        const auto theta = -halfPI + u * doublePI;
        const auto sinTheta = std::sin(theta);
        const auto cosTheta = std::cos(theta);
        
        positions[i] = Vector3F(radius * sinTheta, y * unitHeight - halfHeight, radius * cosTheta);
        normals[i] = Vector3F(sinTheta, 0, cosTheta);
        uvs[i] = Vector2F(u, 1 - v);
    }
    
    for (size_t i = 0; i < torsoRectangleCount; ++i) {
        const auto x = i % radialSegments;
        const auto y = size_t(float(i) * radialSegmentsReciprocal) | 0;
        
        const auto a = y * radialCount + x;
        const auto b = a + 1;
        const auto c = a + radialCount;
        const auto d = c + 1;
        
        indices[indicesOffset++] = static_cast<uint32_t>(b);
        indices[indicesOffset++] = static_cast<uint32_t>(c);
        indices[indicesOffset++] = static_cast<uint32_t>(a);
        indices[indicesOffset++] = static_cast<uint32_t>(b);
        indices[indicesOffset++] = static_cast<uint32_t>(d);
        indices[indicesOffset++] = static_cast<uint32_t>(c);
    }
    
    PrimitiveMesh::_createCapsuleCap(radius,
                                     height,
                                     radialSegments,
                                     doublePI,
                                     torsoVertexCount,
                                     1,
                                     positions,
                                     normals,
                                     uvs,
                                     indices,
                                     indicesOffset
                                     );
    
    PrimitiveMesh::_createCapsuleCap(radius,
                                     height,
                                     radialSegments,
                                     -doublePI,
                                     torsoVertexCount + capVertexCount,
                                     -1,
                                     positions,
                                     normals,
                                     uvs,
                                     indices,
                                     indicesOffset + 6 * capRectangleCount
                                     );
    
    auto &bounds = mesh->bounds;
    bounds.lowerCorner = Point3F(-radius, -radius - halfHeight, -radius);
    bounds.upperCorner = Point3F(radius, radius + halfHeight, radius);
    
    PrimitiveMesh::_initialize(device, mesh, positions, normals, uvs, indices, noLongerAccessible);
    return mesh;
}

void PrimitiveMesh::_createCapsuleCap(float radius,
                                      float height,
                                      size_t radialSegments,
                                      float capAlphaRange,
                                      size_t offset,
                                      ssize_t posIndex,
                                      std::vector<Vector3F> &positions,
                                      std::vector<Vector3F> &normals,
                                      std::vector<Vector2F> &uvs,
                                      std::vector<uint32_t> &indices,
                                      size_t indicesOffset) {
    const auto radialCount = radialSegments + 1;
    const auto halfHeight = height * 0.5;
    const auto capVertexCount = radialCount * radialCount;
    const auto capRectangleCount = radialSegments * radialSegments;
    const auto radialCountReciprocal = 1.0 / radialCount;
    const auto radialSegmentsReciprocal = 1.0 / radialSegments;
    
    for (size_t i = 0; i < capVertexCount; ++i) {
        const auto x = i % radialCount;
        const auto y = size_t(float(i) * radialCountReciprocal) | 0;
        const auto u = x * radialSegmentsReciprocal;
        const auto v = y * radialSegmentsReciprocal;
        const auto alphaDelta = u * capAlphaRange;
        const auto thetaDelta = (v * M_PI) / 2;
        const auto sinTheta = std::sin(thetaDelta);
        
        const auto posX = -radius * std::cos(alphaDelta) * sinTheta;
        const auto posY = (radius * std::cos(thetaDelta) + halfHeight) * posIndex;
        const auto posZ = radius * std::sin(alphaDelta) * sinTheta;
        
        const auto index = i + offset;
        positions[index] = Vector3F(posX, posY, posZ);
        normals[index] = Vector3F(posX, posY, posZ);
        uvs[index] = Vector2F(u, v);
    }
    
    for (size_t i = 0; i < capRectangleCount; ++i) {
        const auto x = i % radialSegments;
        const auto y = size_t(float(i) * radialSegmentsReciprocal) | 0;
        
        const auto a = y * radialCount + x + offset;
        const auto b = a + 1;
        const auto c = a + radialCount;
        const auto d = c + 1;
        
        indices[indicesOffset++] = static_cast<uint32_t>(b);
        indices[indicesOffset++] = static_cast<uint32_t>(a);
        indices[indicesOffset++] = static_cast<uint32_t>(d);
        indices[indicesOffset++] = static_cast<uint32_t>(a);
        indices[indicesOffset++] = static_cast<uint32_t>(c);
        indices[indicesOffset++] = static_cast<uint32_t>(d);
    }
}

void PrimitiveMesh::_initialize(MTL::Device *device,
                                const ModelMeshPtr &mesh,
                                const std::vector<Vector3F> &positions,
                                const std::vector<Vector3F> &normals,
                                std::vector<Vector2F> &uvs,
                                const std::vector<uint32_t> &indices,
                                bool noLongerAccessible) {
    mesh->setPositions(positions);
    mesh->setNormals(normals);
    mesh->setUVs(uvs);
    mesh->setIndices(indices);
    mesh->uploadData(noLongerAccessible);
}

//MARK: -
namespace {
void packVertexData(void *output, VertexFormat format, vector_float4 value) {
    switch (format) {
        case VertexFormatUChar4Normalized:
            ((uint8_t *) output)[3] = 0xFF * value.w;
        case VertexFormatUChar3Normalized:
            ((uint8_t *) output)[2] = 0xFF * value.z;
        case VertexFormatUChar2Normalized:
            ((uint8_t *) output)[1] = 0xFF * value.y;
            ((uint8_t *) output)[0] = 0xFF * value.x;
            break;
        case VertexFormatChar4Normalized:
            ((int8_t *) output)[3] = 0x7F * (2.0 * value.w - 1.0);
        case VertexFormatChar3Normalized:
            ((int8_t *) output)[2] = 0x7F * (2.0 * value.z - 1.0);
        case VertexFormatChar2Normalized:
            ((int8_t *) output)[1] = 0x7F * (2.0 * value.y - 1.0);
            ((int8_t *) output)[0] = 0x7F * (2.0 * value.x - 1.0);
            break;
        case VertexFormatUShort4Normalized:
            ((uint16_t *) output)[3] = 0xFFFF * (2.0 * value.w - 1.0);
        case VertexFormatUShort3Normalized:
            ((uint16_t *) output)[2] = 0xFFFF * (2.0 * value.z - 1.0);
        case VertexFormatUShort2Normalized:
            ((uint16_t *) output)[1] = 0xFFFF * (2.0 * value.y - 1.0);
            ((uint16_t *) output)[0] = 0xFFFF * (2.0 * value.x - 1.0);
            break;
        case VertexFormatShort4Normalized:
            ((int16_t *) output)[3] = 0x7FFF * (2.0 * value.w - 1.0);
        case VertexFormatShort3Normalized:
            ((int16_t *) output)[2] = 0x7FFF * (2.0 * value.z - 1.0);
        case VertexFormatShort2Normalized:
            ((int16_t *) output)[1] = 0x7FFF * (2.0 * value.y - 1.0);
            ((int16_t *) output)[0] = 0x7FFF * (2.0 * value.x - 1.0);
            break;
        case VertexFormatHalf4:
            ((__fp16 *) output)[3] = value.w;
        case VertexFormatHalf3:
            ((__fp16 *) output)[2] = value.z;
        case VertexFormatHalf2:
            ((__fp16 *) output)[1] = value.y;
            ((__fp16 *) output)[0] = value.x;
            break;
        case VertexFormatFloat4:
            ((float *) output)[3] = value.w;
        case VertexFormatFloat3:
            ((float *) output)[2] = value.z;
        case VertexFormatFloat2:
            ((float *) output)[1] = value.y;
        case VertexFormatFloat:
            ((float *) output)[0] = value.x;
            break;
        default:
            break;
    }
}

} // namespace

MeshPtr PrimitiveMesh::makeSphereMesh(MTL::Device &device,
                                      const MTL::VertexDescriptor &vertexDescriptor,
                                      int radialSegments, int verticalSegments, float radius) {
    const UInteger vertexCount = 2 + (radialSegments) * (verticalSegments - 1);
    const UInteger indexCount = 6 * radialSegments * (verticalSegments - 1);;
    
    const UInteger indexBufferSize = indexCount * sizeof(ushort);
    
    assert(vertexCount < UINT16_MAX);
    
    std::vector<MeshBuffer> vertexBuffers;
    
    vertexBuffers = MeshBuffer::makeVertexBuffers(device,
                                                  vertexDescriptor,
                                                  vertexCount,
                                                  indexBufferSize);
    
    Buffer metalBuffer = vertexBuffers[0].buffer();
    
    // Create index buffer from the Metal buffer shared with the vertices and resrve space a the
    // beginning for indices
    MeshBuffer indexBuffer(metalBuffer, 0, indexBufferSize);
    
    uint8_t *bufferContents = (uint8_t *) metalBuffer.contents();
    
    // Fill IndexBuffer
    {
        ushort *indicies = (ushort *) bufferContents;
        
        UInteger currentIndex = 0;
        
        // Indices for top of sphere
        for (ushort phi = 0; phi < radialSegments; phi++) {
            if (phi < radialSegments - 1) {
                indicies[currentIndex++] = 0;
                indicies[currentIndex++] = 2 + phi;
                indicies[currentIndex++] = 1 + phi;
            } else {
                indicies[currentIndex++] = 0;
                indicies[currentIndex++] = 1;
                indicies[currentIndex++] = 1 + phi;
            }
        }
        
        // Indices middle of sphere
        for (ushort theta = 0; theta < verticalSegments - 2; theta++) {
            ushort topRight;
            ushort topLeft;
            ushort bottomRight;
            ushort bottomLeft;
            
            for (ushort phi = 0; phi < radialSegments; phi++) {
                if (phi < radialSegments - 1) {
                    topRight = 1 + theta * (radialSegments) + phi;
                    topLeft = 1 + theta * (radialSegments) + (phi + 1);
                    bottomRight = 1 + (theta + 1) * (radialSegments) + phi;
                    bottomLeft = 1 + (theta + 1) * (radialSegments) + (phi + 1);
                } else {
                    topRight = 1 + theta * (radialSegments) + phi;
                    topLeft = 1 + theta * (radialSegments);
                    bottomRight = 1 + (theta + 1) * (radialSegments) + phi;
                    bottomLeft = 1 + (theta + 1) * (radialSegments);
                }
                
                indicies[currentIndex++] = topRight;
                indicies[currentIndex++] = bottomLeft;
                indicies[currentIndex++] = bottomRight;
                
                indicies[currentIndex++] = topRight;
                indicies[currentIndex++] = topLeft;
                indicies[currentIndex++] = bottomLeft;
            }
        }
        
        // Indicies for bottom of sphere
        ushort lastIndex = radialSegments * (verticalSegments - 1) + 1;
        for (ushort phi = 0; phi < radialSegments; phi++) {
            if (phi < radialSegments - 1) {
                indicies[currentIndex++] = lastIndex;
                indicies[currentIndex++] = lastIndex - radialSegments + phi;
                indicies[currentIndex++] = lastIndex - radialSegments + phi + 1;
            } else {
                indicies[currentIndex++] = lastIndex;
                indicies[currentIndex++] = lastIndex - radialSegments + phi;
                indicies[currentIndex++] = lastIndex - radialSegments;
            }
        }
    }
    
    // Fill positions and normals
    {
        VertexFormat postitionFormat = vertexDescriptor.attributes[VertexAttributePosition].format();
        UInteger positionBufferIndex = vertexDescriptor.attributes[VertexAttributePosition].bufferIndex();
        UInteger positionVertexOffset = vertexDescriptor.attributes[VertexAttributePosition].offset();
        UInteger positionBufferOffset = vertexBuffers[positionBufferIndex].offset();
        UInteger positionStride = vertexDescriptor.layouts[positionBufferIndex].stride();
        
        VertexFormat normalFormat = vertexDescriptor.attributes[VertexAttributeNormal].format();
        UInteger normalBufferIndex = vertexDescriptor.attributes[VertexAttributeNormal].bufferIndex();
        UInteger normalVertexOffset = vertexDescriptor.attributes[VertexAttributeNormal].offset();
        UInteger normalBufferOffset = vertexBuffers[normalBufferIndex].offset();
        UInteger normalStride = vertexDescriptor.layouts[normalBufferIndex].stride();
        
        const double radialDelta = 2 * (M_PI / radialSegments);
        const double verticalDelta = (M_PI / verticalSegments);
        
        uint8_t *positionData = bufferContents + positionBufferOffset + positionVertexOffset;
        uint8_t *normalData = bufferContents + normalBufferOffset + normalVertexOffset;
        
        vector_float4 vertexPosition = {0, radius, 0, 1};
        vector_float4 vertexNormal = {0, 1, 0, 1};;
        
        packVertexData(positionData, postitionFormat, vertexPosition);
        packVertexData(normalData, normalFormat, vertexNormal);
        
        positionData += positionStride;
        normalData += normalStride;
        
        for (ushort verticalSegment = 1; verticalSegment < verticalSegments; verticalSegment++) {
            const double verticalPosition = verticalSegment * verticalDelta;
            
            float y = cos(verticalPosition);
            
            for (ushort radialSegment = 0; radialSegment < radialSegments; radialSegment++) {
                const double radialPositon = radialSegment * radialDelta;
                
                vector_float4 unscaledPositon;
                
                unscaledPositon.x = sin(verticalPosition) * cos(radialPositon);
                unscaledPositon.y = y;
                unscaledPositon.z = sin(verticalPosition) * sin(radialPositon);
                unscaledPositon.w = 1.0;
                
                vertexPosition = radius * unscaledPositon;
                vertexNormal = unscaledPositon;
                
                packVertexData(positionData, postitionFormat, vertexPosition);
                packVertexData(normalData, normalFormat, vertexNormal);
                
                positionData += positionStride;
                normalData += normalStride;
                
            }
        }
        
        vertexPosition = {0, -radius, 0, 1};
        vertexNormal = {0, -1, 0, 1};;
        
        packVertexData(positionData, postitionFormat, vertexPosition);
        packVertexData(normalData, normalFormat, vertexNormal);
        
        positionData += positionStride;
        normalData += normalStride;
        
    }
    
    Submesh submesh(PrimitiveTypeTriangle,
                    IndexTypeUInt16,
                    indexCount,
                    indexBuffer);
    
    return std::make_shared<Mesh>(submesh, vertexBuffers, vertexDescriptor);
}


MeshPtr PrimitiveMesh::makeIcosahedronMesn(MTL::Device &device,
                                           const MTL::VertexDescriptor &vertexDescriptor,
                                           float radius) {
    const float Z = radius;
    const float X = (Z / (1.0 + sqrtf(5.0))) * 2;
    const vector_float4 positions[] =
    {
        {-X, 0.0, Z},
        {X, 0.0, Z},
        {-X, 0.0, -Z},
        {X, 0.0, -Z},
        {0.0, Z, X},
        {0.0, Z, -X},
        {0.0, -Z, X},
        {0.0, -Z, -X},
        {Z, X, 0.0},
        {-Z, X, 0.0},
        {Z, -X, 0.0},
        {-Z, -X, 0.0}
    };
    
    const uint16_t vertexCount = sizeof(positions) / sizeof(vector_float3);
    
    const uint16_t indices[][3] =
    {
        {0, 1, 4},
        {0, 4, 9},
        {9, 4, 5},
        {4, 8, 5},
        {4, 1, 8},
        {8, 1, 10},
        {8, 10, 3},
        {5, 8, 3},
        {5, 3, 2},
        {2, 3, 7},
        {7, 3, 10},
        {7, 10, 6},
        {7, 6, 11},
        {11, 6, 0},
        {0, 6, 1},
        {6, 10, 1},
        {9, 11, 0},
        {9, 2, 11},
        {9, 5, 2},
        {7, 11, 2}
    };
    
    UInteger indexCount = sizeof(indices) / sizeof(uint16_t);
    UInteger indexBufferSize = sizeof(indices);
    
    std::vector<MeshBuffer> vertexBuffers = MeshBuffer::makeVertexBuffers(device, vertexDescriptor,
                                                                          vertexCount, indexBufferSize);
    
    MTL::Buffer buffer = vertexBuffers[0].buffer();
    
    MeshBuffer indexBuffer(buffer, 0, indexBufferSize);
    
    uint8_t *bufferContents = (uint8_t *) buffer.contents();
    
    memcpy(bufferContents, indices, indexBufferSize);
    
    {
        VertexFormat postitionFormat = vertexDescriptor.attributes[VertexAttributePosition].format();
        UInteger positionBufferIndex = vertexDescriptor.attributes[VertexAttributePosition].bufferIndex();
        UInteger positionVertexOffset = vertexDescriptor.attributes[VertexAttributePosition].offset();
        UInteger positionBufferOffset = vertexBuffers[positionBufferIndex].offset();
        UInteger positionStride = vertexDescriptor.layouts[positionBufferIndex].stride();
        
        
        uint8_t *positionData = bufferContents + positionBufferOffset + positionVertexOffset;
        
        for (uint16_t vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++) {
            packVertexData(positionData, postitionFormat, positions[vertexIndex]);
            positionData += positionStride;
        }
    }
    
    Submesh submesh(PrimitiveTypeTriangle,
                    IndexTypeUInt16,
                    indexCount,
                    indexBuffer);
    
    return std::make_shared<Mesh>(submesh, vertexBuffers, vertexDescriptor);
}

}
