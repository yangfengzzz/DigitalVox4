//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include <MetalKit/MetalKit.h>
#include <ModelIO/ModelIO.h>
#include <unordered_map>
#include <set>
#include "core/cpp_mtl_assert.h"

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"
#include "modelio_loader.h"
#include "loader/texture_loader.h"
#include "material/blinn_phong_material.h"
#include "graphics/mesh.h"
#include "mesh/mesh_renderer.h"

using namespace MTL;

namespace vox {
namespace {
TexturePtr createTextureFromMaterial(MDLMaterial *material,
                                     MDLMaterialSemantic materialSemantic,
                                     TextureLoader &textureLoader) {
    NSArray<MDLMaterialProperty *> *propertiesWithSemantic =
    [material propertiesWithSemantic:materialSemantic];
    
    for (MDLMaterialProperty *property in propertiesWithSemantic) {
        if (property.type == MDLMaterialPropertyTypeString ||
            property.type == MDLMaterialPropertyTypeURL) {
            // Load the textures with shader read using private storage
            TextureLoaderOptions options;
            options.usage = MTL::TextureUsageShaderRead;
            options.storageMode = MTL::StorageModePrivate;
            
            // First will interpret the string as a file path and attempt to load it with
            //    -[MTKTextureLoader newTextureWithContentsOfURL:options:error:]
            
            NSURL *url = property.URLValue;
            NSMutableString *URLString = nil;
            if (property.type == MDLMaterialPropertyTypeURL) {
                URLString = [[NSMutableString alloc] initWithString:[url absoluteString]];
            } else {
                URLString = [[NSMutableString alloc] initWithString:@"file://"];
                [URLString appendString:property.URLValue.absoluteString];
            }
            
            // Attempt to load the texture from the file system
            MTL::TexturePtr texture = textureLoader.makeTexture(URLString.UTF8String,
                                                                options);
            
            // If the texture has been found for a material using the string as a file path name...
            if (texture->objCObj()) {
                // ...return it
                return texture;
            }
            
            // If not texture has been fround by interpreting the URL as a path, attempt to load it
            // from the asset catalog by interpreting the string as an asset catalog resource name
            
            texture = textureLoader.makeTexture(property.stringValue.UTF8String,
                                                1.0,
                                                options);
            
            // If a texture with the by interpreting the URL as an asset catalog name
            if (texture->objCObj()) {
                // ...return it
                return texture;
            }
            
            // If did not find the texture in by interpreting it as a file path or as an asset name
            // in the asset catalog, something went wrong (Perhaps the file was missing or
            // misnamed in the asset catalog, model/material file, or file system)
            
            // Depending on how the Metal render pipeline use with this submesh is implemented,
            // this condition can be handled more gracefully.  The app could load a dummy texture
            // that will look okay when set with the pipeline or ensure that the pipelines
            // rendering this submesh do not require a material with this property.
            
            [NSException raise:@"Texture data for material property not found"
                        format:@"Requested material property semantic: %lu string: %@",
             materialSemantic, property.stringValue];
        }
    }
    
    [NSException raise:@"No appropriate material property from which to create texture"
                format:@"Requested material property semantic: %lu", materialSemantic];
    
    return nullptr;
}

std::pair<Submesh, std::shared_ptr<BlinnPhongMaterial>>
createSubmesh(MDLSubmesh *modelIOSubmesh,
              MTKSubmesh *metalKitSubmesh,
              MTL::Device &device,
              TextureLoader &textureLoader) {
    auto mat = std::make_shared<BlinnPhongMaterial>();
    mat->setBaseTexture(createTextureFromMaterial(modelIOSubmesh.material,
                                                  MDLMaterialSemanticBaseColor,
                                                  textureLoader));
    mat->setSpecularTexture(createTextureFromMaterial(modelIOSubmesh.material,
                                                      MDLMaterialSemanticSpecular,
                                                      textureLoader));
    mat->setNormalTexture(createTextureFromMaterial(modelIOSubmesh.material,
                                                    MDLMaterialSemanticTangentSpaceNormal,
                                                    textureLoader));
    
    MTL::Buffer metalIndexBuffer(metalKitSubmesh.indexBuffer.buffer, device);
    MeshBuffer indexBuffer(metalIndexBuffer, metalKitSubmesh.indexBuffer.offset, metalKitSubmesh.indexBuffer.length);
    
    Submesh submesh((PrimitiveType) metalKitSubmesh.primitiveType,
                    (IndexType) metalKitSubmesh.indexType,
                    metalKitSubmesh.indexCount,
                    indexBuffer);
    
    return std::make_pair(submesh, mat);
}


void createMeshFromModelIOMesh(EntityPtr entity,
                               MDLMesh *modelIOMesh,
                               MDLVertexDescriptor *vertexDescriptor,
                               TextureLoader &textureLoader,
                               MTL::Device &device,
                               NSError *__nullable *__nullable error) {
    
    // Have ModelIO create the tangents from mesh texture coordinates and normals
    [modelIOMesh addTangentBasisForTextureCoordinateAttributeNamed:MDLVertexAttributeTextureCoordinate
                                              normalAttributeNamed:MDLVertexAttributeNormal
                                             tangentAttributeNamed:MDLVertexAttributeTangent];
    
    // Have ModelIO create bitangents from mesh texture coordinates and the newly created tangents
    [modelIOMesh addTangentBasisForTextureCoordinateAttributeNamed:MDLVertexAttributeTextureCoordinate
                                             tangentAttributeNamed:MDLVertexAttributeTangent
                                           bitangentAttributeNamed:MDLVertexAttributeBitangent];
    
    // Apply the ModelIO vertex descriptor that the renderer created to match the Metal vertex descriptor.
    
    // Assigning a new vertex descriptor to a ModelIO mesh performs a re-layout of the vertex
    // vertex data.  In this case, rthe renderer created the ModelIO vertex descriptor so that the
    // layout of the vertices in the ModelIO mesh match the layout of vertices the Metal render
    // pipeline expects as input into its vertex shader
    
    // Note ModelIO must create tangents and bitangents (as done above) before this relayout occur
    // This is because Model IO's addTangentBasis methods only works with vertex data is all in
    // 32-bit floating-point.  The vertex descriptor applied, changes those floats into 16-bit
    // floats or other types from which ModelIO cannot produce tangents
    
    modelIOMesh.vertexDescriptor = vertexDescriptor;
    
    
    std::vector<MeshBuffer> vertexBuffers;
    // Create the metalKit mesh which will contain the Metal buffer(s) with the mesh's vertex data
    //   and submeshes with info to draw the mesh
    MTKMesh *metalKitMesh = [[MTKMesh alloc] initWithMesh:modelIOMesh
                                                   device:device.objCObj()
                                                    error:error];
    
    for (NSUInteger argumentIndex = 0; argumentIndex < metalKitMesh.vertexBuffers.count; argumentIndex++) {
        MTKMeshBuffer *mtkMeshBuffer = metalKitMesh.vertexBuffers[argumentIndex];
        if ((NSNull *) mtkMeshBuffer != [NSNull null]) {
            Buffer buffer(mtkMeshBuffer.buffer, device);
            
            MeshBuffer meshBuffer(buffer,
                                  mtkMeshBuffer.offset,
                                  mtkMeshBuffer.length,
                                  argumentIndex);
            
            vertexBuffers.push_back(meshBuffer);
        }
    }
    
    std::vector<Submesh> submeshes;
    std::vector<MaterialPtr> materials;
    // Create an Submesh object for each submesh and a add it to the submesh's array
    for (NSUInteger index = 0; index < metalKitMesh.submeshes.count; index++) {
        // Create an app specific submesh to hold the MetalKit submesh
        auto submesh = createSubmesh(modelIOMesh.submeshes[index],
                                     metalKitMesh.submeshes[index],
                                     device,
                                     textureLoader);
        
        submeshes.emplace_back(submesh.first);
        materials.emplace_back(submesh.second);
    }
    
    auto mtlVertexDescriptor = MTKMetalVertexDescriptorFromModelIO(vertexDescriptor);
    auto desc = MTL::VertexDescriptor();
    // Positions.
    desc.attributes[VertexAttributePosition].format((MTL::VertexFormat)mtlVertexDescriptor.attributes[VertexAttributePosition].format);
    desc.attributes[VertexAttributePosition].offset(mtlVertexDescriptor.attributes[VertexAttributePosition].offset);
    desc.attributes[VertexAttributePosition].bufferIndex(mtlVertexDescriptor.attributes[VertexAttributePosition].bufferIndex);
    
    // Texture coordinates.
    desc.attributes[VertexAttributeTexcoord].format((MTL::VertexFormat)mtlVertexDescriptor.attributes[VertexAttributeTexcoord].format);
    desc.attributes[VertexAttributeTexcoord].offset(mtlVertexDescriptor.attributes[VertexAttributeTexcoord].offset);
    desc.attributes[VertexAttributeTexcoord].bufferIndex(mtlVertexDescriptor.attributes[VertexAttributeTexcoord].bufferIndex);
    
    // Normals.
    desc.attributes[VertexAttributeNormal].format((MTL::VertexFormat)mtlVertexDescriptor.attributes[VertexAttributeNormal].format);
    desc.attributes[VertexAttributeNormal].offset(mtlVertexDescriptor.attributes[VertexAttributeNormal].offset);
    desc.attributes[VertexAttributeNormal].bufferIndex(mtlVertexDescriptor.attributes[VertexAttributeNormal].bufferIndex);
    
    // Tangents
    desc.attributes[VertexAttributeTangent].format((MTL::VertexFormat)mtlVertexDescriptor.attributes[VertexAttributeTangent].format);
    desc.attributes[VertexAttributeTangent].offset(mtlVertexDescriptor.attributes[VertexAttributeTangent].offset);
    desc.attributes[VertexAttributeTangent].bufferIndex(mtlVertexDescriptor.attributes[VertexAttributeTangent].bufferIndex);
    
    // Bitangents
    desc.attributes[VertexAttributeBitangent].format((MTL::VertexFormat)mtlVertexDescriptor.attributes[VertexAttributeBitangent].format);
    desc.attributes[VertexAttributeBitangent].offset(mtlVertexDescriptor.attributes[VertexAttributeBitangent].offset);
    desc.attributes[VertexAttributeBitangent].bufferIndex(mtlVertexDescriptor.attributes[VertexAttributeBitangent].bufferIndex);
    
    // Position Buffer Layout
    desc.layouts[BufferIndexMeshPositions].stride(metalKitMesh.vertexDescriptor.layouts[BufferIndexMeshPositions].stride);
    
    // Generic Attribute Buffer Layout
    desc.layouts[BufferIndexMeshGenerics].stride(metalKitMesh.vertexDescriptor.layouts[BufferIndexMeshGenerics].stride);
    
    auto renderer = entity->addComponent<MeshRenderer>();
    renderer->setMesh(std::make_shared<Mesh>(submeshes, vertexBuffers, desc));
    for (int i = 0; i < materials.size(); i++) {
        renderer->setMaterial(i, materials[i]);
    }
}

void createMeshesFromModelIOObject(EntityPtr entity,
                                   MDLObject *object,
                                   MDLVertexDescriptor *vertexDescriptor,
                                   TextureLoader &textureLoader,
                                   MTL::Device &device,
                                   NSError *__nullable *__nullable error) {
    // If this ModelIO object is a mesh object (not a camera, light, or something else)...
    if ([object isKindOfClass:[MDLMesh class]]) {
        //...create an app-specific Mesh object from it
        MDLMesh *modelIOMesh = (MDLMesh *) object;
        
        createMeshFromModelIOMesh(entity,
                                  modelIOMesh,
                                  vertexDescriptor,
                                  textureLoader,
                                  device,
                                  error);
    }
    
    // Recursively traverse the ModelIO asset hierarchy to find ModelIO meshes that are children
    // of this ModelIO object and create app-specific Mesh objects from those ModelIO meshes
    for (MDLObject *child in object.children) {
        createMeshesFromModelIOObject(entity->createChild(), child,
                                      vertexDescriptor, textureLoader, device, error);
    }
}

} // namespace

//MARK: - Entry
ModelIOLoader::ModelIOLoader(MTL::Device *device):
_device(device) {
    // Positions.
    _defaultVertexDescriptor.attributes[VertexAttributePosition].format(MTL::VertexFormatFloat3);
    _defaultVertexDescriptor.attributes[VertexAttributePosition].offset(0);
    _defaultVertexDescriptor.attributes[VertexAttributePosition].bufferIndex(BufferIndexMeshPositions);
    
    // Texture coordinates.
    _defaultVertexDescriptor.attributes[VertexAttributeTexcoord].format(MTL::VertexFormatFloat2);
    _defaultVertexDescriptor.attributes[VertexAttributeTexcoord].offset(0);
    _defaultVertexDescriptor.attributes[VertexAttributeTexcoord].bufferIndex(BufferIndexMeshGenerics);
    
    // Normals.
    _defaultVertexDescriptor.attributes[VertexAttributeNormal].format(MTL::VertexFormatHalf4);
    _defaultVertexDescriptor.attributes[VertexAttributeNormal].offset(8);
    _defaultVertexDescriptor.attributes[VertexAttributeNormal].bufferIndex(BufferIndexMeshGenerics);
    
    // Tangents
    _defaultVertexDescriptor.attributes[VertexAttributeTangent].format(MTL::VertexFormatHalf4);
    _defaultVertexDescriptor.attributes[VertexAttributeTangent].offset(16);
    _defaultVertexDescriptor.attributes[VertexAttributeTangent].bufferIndex(BufferIndexMeshGenerics);
    
    // Bitangents
    _defaultVertexDescriptor.attributes[VertexAttributeBitangent].format(MTL::VertexFormatHalf4);
    _defaultVertexDescriptor.attributes[VertexAttributeBitangent].offset(24);
    _defaultVertexDescriptor.attributes[VertexAttributeBitangent].bufferIndex(BufferIndexMeshGenerics);
    
    // Position Buffer Layout
    _defaultVertexDescriptor.layouts[BufferIndexMeshPositions].stride(12);
    _defaultVertexDescriptor.layouts[BufferIndexMeshPositions].stepRate(1);
    _defaultVertexDescriptor.layouts[BufferIndexMeshPositions].stepFunction(MTL::VertexStepFunctionPerVertex);
    
    // Generic Attribute Buffer Layout
    _defaultVertexDescriptor.layouts[BufferIndexMeshGenerics].stride(32);
    _defaultVertexDescriptor.layouts[BufferIndexMeshGenerics].stepRate(1);
    _defaultVertexDescriptor.layouts[BufferIndexMeshGenerics].stepFunction(MTL::VertexStepFunctionPerVertex);
}

void ModelIOLoader::loadMesh(const char *bundlePath,
                          const char *meshFile,
                          EntityPtr entity) {
    // Create a ModelIO vertexDescriptor so that the format/layout of the ModelIO mesh vertices
    //   cah be made to match Metal render pipeline's vertex descriptor layout
    MDLVertexDescriptor *modelIOVertexDescriptor =
    MTKModelIOVertexDescriptorFromMetal(_defaultVertexDescriptor.objCObj());
    
    // Indicate how each Metal vertex descriptor attribute maps to each ModelIO attribute
    modelIOVertexDescriptor.attributes[VertexAttributePosition].name = MDLVertexAttributePosition;
    modelIOVertexDescriptor.attributes[VertexAttributeTexcoord].name = MDLVertexAttributeTextureCoordinate;
    modelIOVertexDescriptor.attributes[VertexAttributeNormal].name = MDLVertexAttributeNormal;
    modelIOVertexDescriptor.attributes[VertexAttributeTangent].name = MDLVertexAttributeTangent;
    modelIOVertexDescriptor.attributes[VertexAttributeBitangent].name = MDLVertexAttributeBitangent;
    
    NSString *nsBunldePath = [[NSString alloc] initWithUTF8String:bundlePath];
    NSString *nsMeshName = [[NSString alloc] initWithUTF8String:meshFile];
    NSURL *modelFileURL = [[NSBundle bundleWithPath: nsBunldePath] URLForResource:nsMeshName withExtension:nil];
    
    MTLAssert(modelFileURL, "Could not find model (%s) file in bundle", modelFileURL.absoluteString.UTF8String);
    
    // Create a MetalKit mesh buffer allocator so that ModelIO will load mesh data directly into
    // Metal buffers accessible by the GPU
    MTKMeshBufferAllocator *bufferAllocator =
    [[MTKMeshBufferAllocator alloc] initWithDevice:_device->objCObj()];
    
    // Use ModelIO to load the model file at the URL.  This returns a ModelIO asset object, which
    // contains a hierarchy of ModelIO objects composing a "scene" described by the model file.
    // This hierarchy may include lights, cameras, but, most importantly, mesh and submesh data
    // that we'll render with Metal
    MDLAsset *asset = [[MDLAsset alloc] initWithURL:modelFileURL
                                   vertexDescriptor:nil
                                    bufferAllocator:bufferAllocator];
    
    MTLAssert(asset, "Failed to open model file with given URL: %s", modelFileURL.absoluteString.UTF8String);
    
    // Create a MetalKit texture loader to load material textures from files or the asset catalog
    //   into Metal textures
    TextureLoader textureLoader(*_device);
    NSError *nserror;
    
    // Traverse the ModelIO asset hierarchy to find ModelIO meshes and create app-specific
    // Mesh objects from those ModelIO meshes
    for (MDLObject *object in asset) {
        createMeshesFromModelIOObject(entity, object,
                                      modelIOVertexDescriptor,
                                      textureLoader,
                                      *_device,
                                      &nserror);
    }
    
    MTLAssert(!nserror, (__bridge CFErrorRef) nserror, "");
}


}
