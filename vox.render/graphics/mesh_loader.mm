/*
 See LICENSE folder for this sample’s licensing information.
 
 Abstract:
 Implementation for Mesh and Submesh objects.
 */
#include <MetalKit/MetalKit.h>
#include <ModelIO/ModelIO.h>
#include <unordered_map>
#include <set>

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"
#include "mesh_loader.h"
#include "rendering/utilities.h"
#include "material/texture_loader.h"

using namespace MTL;

namespace vox {
static Texture createTextureFromMaterial(MDLMaterial *material,
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
            MTL::Texture texture = textureLoader.makeTexture(URLString.UTF8String,
                                                             options);
            
            // If the texture has been found for a material using the string as a file path name...
            if (texture.objCObj()) {
                // ...return it
                return texture;
            }
            
            // If not texture has been fround by interpreting the URL as a path, attempt to load it
            // from the asset catalog by interpreting the string as an asset catalog resource name
            
            texture = textureLoader.makeTexture(property.stringValue.UTF8String,
                                                1.0,
                                                options);
            
            // If a texture with the by interpreting the URL as an asset catalog name
            if (texture.objCObj()) {
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
    
    return Texture();
}

static Submesh createSubmesh(MDLSubmesh *modelIOSubmesh,
                             MTKSubmesh *metalKitSubmesh,
                             MTL::Device &device,
                             TextureLoader &textureLoader) {
    
    // Set each index in the array with the appropriate material semantic specified in the
    //   submesh's material property
    
    // Create a vector with 3 dummy (unusable) texture that will be immediate replaced
    std::vector<MTL::Texture> textures(NumMeshTextures, Texture());
    
    // Now that createSubmesh has added dummy elements, it can replace indices in the vector
    // With real textures
    
    textures[TextureIndexBaseColor] = createTextureFromMaterial(modelIOSubmesh.material,
                                                                MDLMaterialSemanticBaseColor,
                                                                textureLoader);
    
    textures[TextureIndexSpecular] = createTextureFromMaterial(modelIOSubmesh.material,
                                                               MDLMaterialSemanticSpecular,
                                                               textureLoader);
    
    textures[TextureIndexNormal] = createTextureFromMaterial(modelIOSubmesh.material,
                                                             MDLMaterialSemanticTangentSpaceNormal,
                                                             textureLoader);
    
    MTL::Buffer metalIndexBuffer(metalKitSubmesh.indexBuffer.buffer, device);
    
    MeshBuffer indexBuffer(metalIndexBuffer, metalKitSubmesh.indexBuffer.offset, metalKitSubmesh.indexBuffer.length);
    
    Submesh submesh((PrimitiveType) metalKitSubmesh.primitiveType,
                    (IndexType) metalKitSubmesh.indexType,
                    metalKitSubmesh.indexCount,
                    indexBuffer,
                    textures);
    
    return submesh;
}


Mesh createMeshFromModelIOMesh(MDLMesh *modelIOMesh,
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
    
    // Create an Submesh object for each submesh and a add it to the submesh's array
    for (NSUInteger index = 0; index < metalKitMesh.submeshes.count; index++) {
        // Create an app specific submesh to hold the MetalKit submesh
        Submesh submesh = createSubmesh(modelIOMesh.submeshes[index],
                                        metalKitMesh.submeshes[index],
                                        device,
                                        textureLoader);
        
        submeshes.emplace_back(submesh);
    }
    
    
    Mesh mesh(submeshes, vertexBuffers);
    
    return mesh;
}

static std::vector<Mesh> createMeshesFromModelIOObject(MDLObject *object,
                                                       MDLVertexDescriptor *vertexDescriptor,
                                                       TextureLoader &textureLoader,
                                                       MTL::Device &device,
                                                       NSError *__nullable *__nullable error) {
    std::vector<Mesh> newMeshes;
    
    // If this ModelIO object is a mesh object (not a camera, light, or something else)...
    if ([object isKindOfClass:[MDLMesh class]]) {
        //...create an app-specific Mesh object from it
        MDLMesh *modelIOMesh = (MDLMesh *) object;
        
        Mesh newMesh = createMeshFromModelIOMesh(modelIOMesh,
                                                 vertexDescriptor,
                                                 textureLoader,
                                                 device,
                                                 error);
        
        newMeshes.emplace_back(newMesh);
    }
    
    // Recursively traverse the ModelIO asset hierarchy to find ModelIO meshes that are children
    // of this ModelIO object and create app-specific Mesh objects from those ModelIO meshes
    for (MDLObject *child in object.children) {
        std::vector<Mesh> childMeshes;
        
        childMeshes = createMeshesFromModelIOObject(child, vertexDescriptor, textureLoader, device, error);
        
        newMeshes.insert(newMeshes.end(), childMeshes.begin(), childMeshes.end());
    }
    
    return newMeshes;
}

std::vector<Mesh> *newMeshesFromBundlePath(const char *bundlePath,
                                           const char *meshFile,
                                           MTL::Device &device,
                                           const MTL::VertexDescriptor &vertexDescriptor,
                                           CFErrorRef *error) {
    // Create a ModelIO vertexDescriptor so that the format/layout of the ModelIO mesh vertices
    //   cah be made to match Metal render pipeline's vertex descriptor layout
    MDLVertexDescriptor *modelIOVertexDescriptor =
    MTKModelIOVertexDescriptorFromMetal(vertexDescriptor.objCObj());
    
    // Indicate how each Metal vertex descriptor attribute maps to each ModelIO attribute
    modelIOVertexDescriptor.attributes[VertexAttributePosition].name = MDLVertexAttributePosition;
    modelIOVertexDescriptor.attributes[VertexAttributeTexcoord].name = MDLVertexAttributeTextureCoordinate;
    modelIOVertexDescriptor.attributes[VertexAttributeNormal].name = MDLVertexAttributeNormal;
    modelIOVertexDescriptor.attributes[VertexAttributeTangent].name = MDLVertexAttributeTangent;
    modelIOVertexDescriptor.attributes[VertexAttributeBitangent].name = MDLVertexAttributeBitangent;
    
    NSString *nsBunldePath = [[NSString alloc] initWithUTF8String:bundlePath];
    NSString *nsMeshName = [[NSString alloc] initWithUTF8String:meshFile];
    NSURL *modelFileURL = [[NSBundle bundleWithPath: nsBunldePath] URLForResource:nsMeshName withExtension:nil];
    
    AAPLAssert(modelFileURL, "Could not find model (%s) file in bundle", modelFileURL.absoluteString.UTF8String);
    
    // Create a MetalKit mesh buffer allocator so that ModelIO will load mesh data directly into
    // Metal buffers accessible by the GPU
    MTKMeshBufferAllocator *bufferAllocator =
    [[MTKMeshBufferAllocator alloc] initWithDevice:device.objCObj()];
    
    // Use ModelIO to load the model file at the URL.  This returns a ModelIO asset object, which
    // contains a hierarchy of ModelIO objects composing a "scene" described by the model file.
    // This hierarchy may include lights, cameras, but, most importantly, mesh and submesh data
    // that we'll render with Metal
    MDLAsset *asset = [[MDLAsset alloc] initWithURL:modelFileURL
                                   vertexDescriptor:nil
                                    bufferAllocator:bufferAllocator];
    
    AAPLAssert(asset, "Failed to open model file with given URL: %s", modelFileURL.absoluteString.UTF8String);
    
    // Create a MetalKit texture loader to load material textures from files or the asset catalog
    //   into Metal textures
    TextureLoader textureLoader(device);
    
    std::vector<Mesh> *newMeshes = new std::vector<Mesh>();
    
    NSError *nserror;
    
    // Traverse the ModelIO asset hierarchy to find ModelIO meshes and create app-specific
    // Mesh objects from those ModelIO meshes
    for (MDLObject *object in asset) {
        std::vector<Mesh> assetMeshes;
        
        assetMeshes = createMeshesFromModelIOObject(object,
                                                    modelIOVertexDescriptor,
                                                    textureLoader,
                                                    device,
                                                    &nserror);
        
        newMeshes->insert(newMeshes->end(), assetMeshes.begin(), assetMeshes.end());
    }
    
    if (nserror && error) {
        *error = (__bridge CFErrorRef) nserror;
    }
    
    return newMeshes;
}


}
