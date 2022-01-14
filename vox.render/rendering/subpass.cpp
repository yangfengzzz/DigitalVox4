/*
 See LICENSE folder for this sample’s licensing information.
 
 Abstract:
 Implementation of subpass class which performs Metal setup and per frame rendering
 */

#include "subpass.h"

#include <sys/sysctl.h>
#include <simd/simd.h>
#include <stdlib.h>
#include <array>
#include <string>

#include "subpass.h"
#include "material/texture_loader.h"
#include "graphics/mesh_loader.h"
#include "graphics/primitive_mesh.h"
#include "math_utilities.h"
#include "utilities.h"

using namespace simd;

// Include header shared between C code here, which executes Metal API commands, and .metal files
#include "shader_types.h"

namespace vox {
// Number of vertices in our 2D fairy model
static const uint32_t NumFairyVertices = 7;

// 30% of lights are around the tree
// 40% of lights are on the ground inside the columns
// 30% of lights are around the outside of the columns
static const uint32_t TreeLights = 0 + 0.30 * NumLights;
static const uint32_t GroundLights = TreeLights + 0.40 * NumLights;
static const uint32_t ColumnLights = GroundLights + 0.30 * NumLights;

Subpass::Subpass(MTL::View* view)
: m_view(view),
m_device(view->device()),
m_completedHandler(nullptr),
m_originalLightPositions(nullptr),
m_frameDataBufferIndex(0),
m_frameNumber(0) {
    
    this->m_inFlightSemaphore = dispatch_semaphore_create(MaxFramesInFlight);
}


Subpass::~Subpass() {
    delete[] m_originalLightPositions;
    
    delete m_meshes;
    
    delete m_completedHandler;
}

/// Create Metal render state objects
void Subpass::loadMetal() {
    // Create and load the basic Metal state objects
    CFErrorRef error = nullptr;
    
    printf("Selected Device: %s\n", m_view->device().name());
    
    for (uint8_t i = 0; i < MaxFramesInFlight; i++) {
        // Indicate shared storage so that both the CPU can access the buffers
        static const MTL::ResourceOptions storageMode = MTL::ResourceStorageModeShared;
        
        m_uniformBuffers[i] = m_device.makeBuffer(sizeof(FrameData), storageMode);
        
        m_uniformBuffers[i].label("UniformBuffer");
        
        m_lightPositions[i] = m_device.makeBuffer(sizeof(float4) * NumLights, storageMode);
        
        m_uniformBuffers[i].label("LightPositions");
    }
    
    MTL::Library shaderLibrary = makeShaderLibrary();
    
    // Positions.
    
    m_defaultVertexDescriptor.attributes[VertexAttributePosition].format(MTL::VertexFormatFloat3);
    m_defaultVertexDescriptor.attributes[VertexAttributePosition].offset(0);
    m_defaultVertexDescriptor.attributes[VertexAttributePosition].bufferIndex(BufferIndexMeshPositions);
    
    // Texture coordinates.
    m_defaultVertexDescriptor.attributes[VertexAttributeTexcoord].format(MTL::VertexFormatFloat2);
    m_defaultVertexDescriptor.attributes[VertexAttributeTexcoord].offset(0);
    m_defaultVertexDescriptor.attributes[VertexAttributeTexcoord].bufferIndex(BufferIndexMeshGenerics);
    
    // Normals.
    m_defaultVertexDescriptor.attributes[VertexAttributeNormal].format(MTL::VertexFormatHalf4);
    m_defaultVertexDescriptor.attributes[VertexAttributeNormal].offset(8);
    m_defaultVertexDescriptor.attributes[VertexAttributeNormal].bufferIndex(BufferIndexMeshGenerics);
    
    // Tangents
    m_defaultVertexDescriptor.attributes[VertexAttributeTangent].format(MTL::VertexFormatHalf4);
    m_defaultVertexDescriptor.attributes[VertexAttributeTangent].offset(16);
    m_defaultVertexDescriptor.attributes[VertexAttributeTangent].bufferIndex(BufferIndexMeshGenerics);
    
    // Bitangents
    m_defaultVertexDescriptor.attributes[VertexAttributeBitangent].format(MTL::VertexFormatHalf4);
    m_defaultVertexDescriptor.attributes[VertexAttributeBitangent].offset(24);
    m_defaultVertexDescriptor.attributes[VertexAttributeBitangent].bufferIndex(BufferIndexMeshGenerics);
    
    // Position Buffer Layout
    m_defaultVertexDescriptor.layouts[BufferIndexMeshPositions].stride(12);
    m_defaultVertexDescriptor.layouts[BufferIndexMeshPositions].stepRate(1);
    m_defaultVertexDescriptor.layouts[BufferIndexMeshPositions].stepFunction(MTL::VertexStepFunctionPerVertex);
    
    // Generic Attribute Buffer Layout
    m_defaultVertexDescriptor.layouts[BufferIndexMeshGenerics].stride(32);
    m_defaultVertexDescriptor.layouts[BufferIndexMeshGenerics].stepRate(1);
    m_defaultVertexDescriptor.layouts[BufferIndexMeshGenerics].stepFunction(MTL::VertexStepFunctionPerVertex);
    
    m_view->depthStencilPixelFormat(MTL::PixelFormatDepth32Float_Stencil8);
    m_view->colorPixelFormat(MTL::PixelFormatBGRA8Unorm_sRGB);
    
    m_albedo_specular_GBufferFormat = MTL::PixelFormatRGBA8Unorm_sRGB;
    m_normal_shadow_GBufferFormat = MTL::PixelFormatRGBA8Snorm;
    m_depth_GBufferFormat = MTL::PixelFormatR32Float;
    
#pragma mark GBuffer render pipeline setup
    {
        {
            MTL::Function GBufferVertexFunction = shaderLibrary.makeFunction("gbuffer_vertex");
            MTL::Function GBufferFragmentFunction = shaderLibrary.makeFunction("gbuffer_fragment");
            
            MTL::RenderPipelineDescriptor renderPipelineDescriptor;
            
            renderPipelineDescriptor.label("G-buffer Creation");
            renderPipelineDescriptor.vertexDescriptor(&m_defaultVertexDescriptor);
            
            if (m_singlePassDeferred) {
                renderPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(m_view->colorPixelFormat());
            } else {
                renderPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(MTL::PixelFormatInvalid);
            }
            
            renderPipelineDescriptor.colorAttachments[RenderTargetAlbedo].pixelFormat(m_albedo_specular_GBufferFormat);
            renderPipelineDescriptor.colorAttachments[RenderTargetNormal].pixelFormat(m_normal_shadow_GBufferFormat);
            renderPipelineDescriptor.colorAttachments[RenderTargetDepth].pixelFormat(m_depth_GBufferFormat);
            renderPipelineDescriptor.depthAttachmentPixelFormat(m_view->depthStencilPixelFormat());
            renderPipelineDescriptor.stencilAttachmentPixelFormat(m_view->depthStencilPixelFormat());
            
            renderPipelineDescriptor.vertexFunction(&GBufferVertexFunction);
            renderPipelineDescriptor.fragmentFunction(&GBufferFragmentFunction);
            
            m_GBufferPipelineState = m_device.makeRenderPipelineState(renderPipelineDescriptor, &error);
            
            AAPLAssert(error == nullptr, error, "Failed to create GBuffer render pipeline state");
        }
        
#pragma mark GBuffer depth state setup
        {
#if LIGHT_STENCIL_CULLING
            MTL::StencilDescriptor stencilStateDesc;
            stencilStateDesc.stencilCompareFunction(MTL::CompareFunctionAlways);
            stencilStateDesc.stencilFailureOperation(MTL::StencilOperationKeep);
            stencilStateDesc.depthFailureOperation(MTL::StencilOperationKeep);
            stencilStateDesc.depthStencilPassOperation(MTL::StencilOperationReplace);
            stencilStateDesc.readMask(0x0);
            stencilStateDesc.writeMask(0xFF);
#else
            MTL::StencilDescriptor stencilStateDesc;
#endif
            MTL::DepthStencilDescriptor depthStencilDesc;
            depthStencilDesc.label("G-buffer Creation");
            depthStencilDesc.depthCompareFunction(MTL::CompareFunctionLess);
            depthStencilDesc.depthWriteEnabled(true);
            depthStencilDesc.frontFaceStencil = stencilStateDesc;
            depthStencilDesc.backFaceStencil = stencilStateDesc;
            
            m_GBufferDepthStencilState = m_device.makeDepthStencilState(depthStencilDesc);
        }
    }
    
    // Setup render state to apply directional light and shadow in final pass
    {
#pragma mark Directional lighting render pipeline setup
        {
            MTL::Function directionalVertexFunction = shaderLibrary.makeFunction("deferred_direction_lighting_vertex");
            MTL::Function directionalFragmentFunction;
            
            if (m_singlePassDeferred) {
                directionalFragmentFunction =
                shaderLibrary.makeFunction("deferred_directional_lighting_fragment_single_pass");
            } else {
                directionalFragmentFunction =
                shaderLibrary.makeFunction("deferred_directional_lighting_fragment_traditional");
            }
            
            MTL::RenderPipelineDescriptor renderPipelineDescriptor;
            
            renderPipelineDescriptor.label("Deferred Directional Lighting");
            renderPipelineDescriptor.vertexDescriptor(nullptr);
            renderPipelineDescriptor.vertexFunction(&directionalVertexFunction);
            renderPipelineDescriptor.fragmentFunction(&directionalFragmentFunction);
            renderPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(m_view->colorPixelFormat());
            
            if (m_singlePassDeferred) {
                renderPipelineDescriptor.colorAttachments[RenderTargetAlbedo].pixelFormat(m_albedo_specular_GBufferFormat);
                renderPipelineDescriptor.colorAttachments[RenderTargetNormal].pixelFormat(m_normal_shadow_GBufferFormat);
                renderPipelineDescriptor.colorAttachments[RenderTargetDepth].pixelFormat(m_depth_GBufferFormat);
            }
            
            renderPipelineDescriptor.depthAttachmentPixelFormat(m_view->depthStencilPixelFormat());
            renderPipelineDescriptor.stencilAttachmentPixelFormat(m_view->depthStencilPixelFormat());
            
            m_directionalLightPipelineState = m_device.makeRenderPipelineState(renderPipelineDescriptor,
                                                                               &error);
            
            AAPLAssert(error == nullptr, error,
                       "Failed to create directional light render pipeline state:");
        }
        
#pragma mark Directional lighting mask depth stencil state setup
        {
#if LIGHT_STENCIL_CULLING
            // Stencil state setup so direction lighting fragment shader only executed on pixels
            // drawn in GBuffer stage (i.e. mask out the background/sky)
            MTL::StencilDescriptor stencilStateDesc;
            stencilStateDesc.stencilCompareFunction(MTL::CompareFunctionEqual);
            stencilStateDesc.stencilFailureOperation(MTL::StencilOperationKeep);
            stencilStateDesc.depthFailureOperation(MTL::StencilOperationKeep);
            stencilStateDesc.depthStencilPassOperation(MTL::StencilOperationKeep);
            stencilStateDesc.readMask(0xFF);
            stencilStateDesc.writeMask(0x0);
#else
            MTL::StencilDescriptor stencilStateDesc;
#endif
            MTL::DepthStencilDescriptor depthStencilDesc;
            depthStencilDesc.label("Deferred Directional Lighting");
            depthStencilDesc.depthWriteEnabled(false);
            depthStencilDesc.depthCompareFunction(MTL::CompareFunctionAlways);
            depthStencilDesc.frontFaceStencil = stencilStateDesc;
            depthStencilDesc.backFaceStencil = stencilStateDesc;
            
            m_directionLightDepthStencilState = m_device.makeDepthStencilState(depthStencilDesc);
        }
    }
    
#pragma mark Fairy billboard render pipeline setup
    {
        MTL::Function fairyVertexFunction = shaderLibrary.makeFunction("fairy_vertex");
        MTL::Function fairyFragmentFunction = shaderLibrary.makeFunction("fairy_fragment");
        
        MTL::RenderPipelineDescriptor renderPipelineDescriptor;
        
        renderPipelineDescriptor.label("Fairy Drawing");
        renderPipelineDescriptor.vertexDescriptor(nullptr);
        renderPipelineDescriptor.vertexFunction(&fairyVertexFunction);
        renderPipelineDescriptor.fragmentFunction(&fairyFragmentFunction);
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(m_view->colorPixelFormat());
        
        // Because iOS subpass can perform GBuffer pass in final pass, any pipeline rendering in
        // the final pass must take the GBuffers into account
        if (m_singlePassDeferred) {
            renderPipelineDescriptor.colorAttachments[RenderTargetAlbedo].pixelFormat(m_albedo_specular_GBufferFormat);
            renderPipelineDescriptor.colorAttachments[RenderTargetNormal].pixelFormat(m_normal_shadow_GBufferFormat);
            renderPipelineDescriptor.colorAttachments[RenderTargetDepth].pixelFormat(m_depth_GBufferFormat);
        }
        
        renderPipelineDescriptor.depthAttachmentPixelFormat(m_view->depthStencilPixelFormat());
        renderPipelineDescriptor.stencilAttachmentPixelFormat(m_view->depthStencilPixelFormat());
        renderPipelineDescriptor.colorAttachments[0].blendingEnabled(true);
        renderPipelineDescriptor.colorAttachments[0].rgbBlendOperation(MTL::BlendOperationAdd);
        renderPipelineDescriptor.colorAttachments[0].alphaBlendOperation(MTL::BlendOperationAdd);
        renderPipelineDescriptor.colorAttachments[0].sourceRGBBlendFactor(MTL::BlendFactorSourceAlpha);
        renderPipelineDescriptor.colorAttachments[0].sourceAlphaBlendFactor(MTL::BlendFactorSourceAlpha);
        renderPipelineDescriptor.colorAttachments[0].destinationRGBBlendFactor(MTL::BlendFactorOne);
        renderPipelineDescriptor.colorAttachments[0].destinationAlphaBlendFactor(MTL::BlendFactorOne);
        
        m_fairyPipelineState = m_device.makeRenderPipelineState(renderPipelineDescriptor);
        
        AAPLAssert(error == nullptr, error, "Failed to create fairy render pipeline state: %@");
    }
    
#pragma mark Sky render pipeline setup
    {
        m_skyVertexDescriptor.attributes[VertexAttributePosition].format(MTL::VertexFormatFloat3);
        m_skyVertexDescriptor.attributes[VertexAttributePosition].offset(0);
        m_skyVertexDescriptor.attributes[VertexAttributePosition].bufferIndex(BufferIndexMeshPositions);
        m_skyVertexDescriptor.layouts[BufferIndexMeshPositions].stride(12);
        m_skyVertexDescriptor.attributes[VertexAttributeNormal].format(MTL::VertexFormatFloat3);
        m_skyVertexDescriptor.attributes[VertexAttributeNormal].offset(0);
        m_skyVertexDescriptor.attributes[VertexAttributeNormal].bufferIndex(BufferIndexMeshGenerics);
        m_skyVertexDescriptor.layouts[BufferIndexMeshGenerics].stride(12);
        
        MTL::Function skyboxVertexFunction = shaderLibrary.makeFunction("skybox_vertex");
        MTL::Function skyboxFragmentFunction = shaderLibrary.makeFunction("skybox_fragment");
        
        MTL::RenderPipelineDescriptor renderPipelineDescriptor;
        renderPipelineDescriptor.label("Sky");
        renderPipelineDescriptor.vertexDescriptor(&m_skyVertexDescriptor);
        renderPipelineDescriptor.vertexFunction(&skyboxVertexFunction);
        renderPipelineDescriptor.fragmentFunction(&skyboxFragmentFunction);
        renderPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(m_view->colorPixelFormat());
        
        if (m_singlePassDeferred) {
            renderPipelineDescriptor.colorAttachments[RenderTargetAlbedo].pixelFormat(m_albedo_specular_GBufferFormat);
            renderPipelineDescriptor.colorAttachments[RenderTargetNormal].pixelFormat(m_normal_shadow_GBufferFormat);
            renderPipelineDescriptor.colorAttachments[RenderTargetDepth].pixelFormat(m_depth_GBufferFormat);
        }
        
        renderPipelineDescriptor.depthAttachmentPixelFormat(m_view->depthStencilPixelFormat());
        renderPipelineDescriptor.stencilAttachmentPixelFormat(m_view->depthStencilPixelFormat());
        
        m_skyboxPipelineState = m_device.makeRenderPipelineState(renderPipelineDescriptor, &error);
        
        AAPLAssert(error == nullptr, error, "Failed to create skybox render pipeline state: %@");
    }
    
#pragma mark Post lighting depth state setup
    {
        MTL::DepthStencilDescriptor depthStencilDesc;
        depthStencilDesc.label("Less -Writes");
        depthStencilDesc.depthCompareFunction(MTL::CompareFunctionLess);
        depthStencilDesc.depthWriteEnabled(false);
        
        m_dontWriteDepthStencilState = m_device.newDepthStencilStateWithDescriptor(depthStencilDesc);
    }
    
    // Setup objects for shadow pass
    {
        MTL::PixelFormat shadowMapPixelFormat = MTL::PixelFormatDepth16Unorm;
        
#pragma mark Shadow pass render pipeline setup
        {
            MTL::Function *shadowVertexFunction = shaderLibrary.newFunctionWithName("shadow_vertex");
            
            MTL::RenderPipelineDescriptor renderPipelineDescriptor;
            renderPipelineDescriptor.label("Shadow Gen");
            renderPipelineDescriptor.vertexDescriptor(nullptr);
            renderPipelineDescriptor.vertexFunction(shadowVertexFunction);
            renderPipelineDescriptor.fragmentFunction(nullptr);
            renderPipelineDescriptor.depthAttachmentPixelFormat(shadowMapPixelFormat);
            
            m_shadowGenPipelineState = m_device.makeRenderPipelineState(renderPipelineDescriptor, &error);
            
            delete shadowVertexFunction;
        }
        
#pragma mark Shadow pass depth state setup
        {
            MTL::DepthStencilDescriptor depthStencilDesc;
            depthStencilDesc.label("Shadow Gen");
            depthStencilDesc.depthCompareFunction(MTL::CompareFunctionLessEqual);
            depthStencilDesc.depthWriteEnabled(true);
            m_shadowDepthStencilState = m_device.makeDepthStencilState(depthStencilDesc);
        }
        
#pragma mark Shadow map setup
        {
            MTL::TextureDescriptor shadowTextureDesc;
            
            shadowTextureDesc.pixelFormat(shadowMapPixelFormat);
            shadowTextureDesc.width(2048);
            shadowTextureDesc.height(2048);
            shadowTextureDesc.mipmapLevelCount(1);
            shadowTextureDesc.resourceOptions(MTL::ResourceStorageModePrivate);
            shadowTextureDesc.usage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
            
            m_shadowMap = m_device.makeTexture(shadowTextureDesc);
            m_shadowMap.label("Shadow Map");
        }
        
#pragma mark Shadow render pass descriptor setup
        {
            m_shadowRenderPassDescriptor.depthAttachment.texture(m_shadowMap);
            m_shadowRenderPassDescriptor.depthAttachment.loadAction(MTL::LoadActionClear);
            m_shadowRenderPassDescriptor.depthAttachment.storeAction(MTL::StoreActionStore);
            m_shadowRenderPassDescriptor.depthAttachment.clearDepth(1.0);
        }
        
        // Calculate projection matrix to render shadows
        {
            m_shadowProjectionMatrix = matrix_ortho_left_hand(-53, 53, -33, 53, -53, 53);
        }
    }
    
#if LIGHT_STENCIL_CULLING
    // Setup objects for point light mask rendering
    {
#pragma mark Light mask render pipeline state setup
        {
            MTL::Function lightMaskVertex = shaderLibrary.makeFunction("light_mask_vertex");
            
            MTL::RenderPipelineDescriptor renderPipelineDescriptor;
            renderPipelineDescriptor.label("Point Light Mask");
            renderPipelineDescriptor.vertexDescriptor(nullptr);
            renderPipelineDescriptor.vertexFunction(&lightMaskVertex);
            renderPipelineDescriptor.fragmentFunction(nullptr);
            renderPipelineDescriptor.colorAttachments[RenderTargetLighting].pixelFormat(m_view->colorPixelFormat());
            
            if (m_singlePassDeferred) {
                renderPipelineDescriptor.colorAttachments[RenderTargetAlbedo].pixelFormat(m_albedo_specular_GBufferFormat);
                renderPipelineDescriptor.colorAttachments[RenderTargetNormal].pixelFormat(m_normal_shadow_GBufferFormat);
                renderPipelineDescriptor.colorAttachments[RenderTargetDepth].pixelFormat(m_depth_GBufferFormat);
            }
            
            renderPipelineDescriptor.depthAttachmentPixelFormat(m_view->depthStencilPixelFormat());
            renderPipelineDescriptor.stencilAttachmentPixelFormat(m_view->depthStencilPixelFormat());
            
            m_lightMaskPipelineState =
            m_device.makeRenderPipelineState(renderPipelineDescriptor, &error);
            
            AAPLAssert(error == nullptr, error,
                       "Failed to create directional light mask pipeline state:");
        }
        
#pragma mark Light mask depth stencil state setup
        {
            MTL::StencilDescriptor stencilStateDesc;
            stencilStateDesc.stencilCompareFunction(MTL::CompareFunctionAlways);
            stencilStateDesc.stencilFailureOperation(MTL::StencilOperationKeep);
            stencilStateDesc.depthFailureOperation(MTL::StencilOperationIncrementClamp);
            stencilStateDesc.depthStencilPassOperation(MTL::StencilOperationKeep);
            stencilStateDesc.readMask(0x0);
            stencilStateDesc.writeMask(0xFF);
            MTL::DepthStencilDescriptor depthStencilDesc;
            depthStencilDesc.label("Point Light Mask");
            depthStencilDesc.depthWriteEnabled(false);
            depthStencilDesc.depthCompareFunction(MTL::CompareFunctionLessEqual);
            depthStencilDesc.frontFaceStencil = stencilStateDesc;
            depthStencilDesc.backFaceStencil = stencilStateDesc;
            
            m_lightMaskDepthStencilState = m_device.makeDepthStencilState(depthStencilDesc);
        }
    }
#endif // END LIGHT_STENCIL_CULLING
    
#pragma mark Point light depth state setup
    {
#if LIGHT_STENCIL_CULLING
        MTL::StencilDescriptor stencilStateDesc;
        stencilStateDesc.stencilCompareFunction(MTL::CompareFunctionLess);
        stencilStateDesc.stencilFailureOperation(MTL::StencilOperationKeep);
        stencilStateDesc.depthFailureOperation(MTL::StencilOperationKeep);
        stencilStateDesc.depthStencilPassOperation(MTL::StencilOperationKeep);
        stencilStateDesc.readMask(0xFF);
        stencilStateDesc.writeMask(0x0);
#else  // IF NOT LIGHT_STENCIL_CULLING
        MTL::StencilDescriptor stencilStateDesc;
#endif // END NOT LIGHT_STENCIL_CULLING
        MTL::DepthStencilDescriptor depthStencilDesc;
        depthStencilDesc.depthWriteEnabled(false);
        depthStencilDesc.depthCompareFunction(MTL::CompareFunctionLessEqual);
        depthStencilDesc.frontFaceStencil = stencilStateDesc;
        depthStencilDesc.backFaceStencil = stencilStateDesc;
        depthStencilDesc.label("Point Light");
        
        m_pointLightDepthStencilState = m_device.makeDepthStencilState(depthStencilDesc);
    }
    
    m_commandQueue = m_device.makeCommandQueue();
}

/// Load models/textures, etc.
void Subpass::loadScene() {
    // Create and load assets into Metal objects including meshes and textures
    CFErrorRef error = nullptr;
    
    m_meshes = newMeshesFromBundlePath("../assets/Models", "Temple.obj",
                                       m_device, m_defaultVertexDescriptor, &error);
    
    AAPLAssert(m_meshes, error, "Could not create meshes from model file");
    
    // Generate data
    {
        m_lightsData = m_device.makeBuffer(sizeof(PointLight) * NumLights);
        
        m_lightsData.label("LightData");
        
        populateLights();
    }
    
    // Create quad for fullscreen composition drawing
    {
        static const SimpleVertex QuadVertices[] =
        {
            {{-1.0f, -1.0f,}},
            {{-1.0f, 1.0f,}},
            {{1.0f, -1.0f,}},
            
            {{1.0f, -1.0f,}},
            {{-1.0f, 1.0f,}},
            {{1.0f, 1.0f,}},
        };
        
        m_quadVertexBuffer = m_device.makeBuffer(QuadVertices, sizeof(QuadVertices));
        
        m_quadVertexBuffer.label("Quad Vertices");
    }
    
    // Create a simple 2D triangle strip circle mesh for fairies
    {
        SimpleVertex fairyVertices[NumFairyVertices];
        const float angle = 2 * M_PI / (float) NumFairyVertices;
        for (int vtx = 0; vtx < NumFairyVertices; vtx++) {
            int point = (vtx % 2) ? (vtx + 1) / 2 : -vtx / 2;
            float2 position = {sin(point * angle), cos(point * angle)};
            fairyVertices[vtx].position = position;
        }
        
        m_fairy = m_device.makeBuffer(fairyVertices, sizeof(fairyVertices));
        
        m_fairy.label("Fairy Vertices");
    }
    
    // Create an icosahedron mesh for fairy light volumes
    {
        // Create vertex descriptor with layout for icoshedron
        MTL::VertexDescriptor icosahedronDescriptor;
        icosahedronDescriptor.attributes[VertexAttributePosition].format(MTL::VertexFormatFloat4);
        icosahedronDescriptor.attributes[VertexAttributePosition].offset(0);
        icosahedronDescriptor.attributes[VertexAttributePosition].bufferIndex(BufferIndexMeshPositions);
        
        icosahedronDescriptor.layouts[BufferIndexMeshPositions].stride(sizeof(float4));
        
        // Calculate radius such that minium radius of icosahedronDescriptor is 1
        const float icoshedronRadius = 1.0 / (sqrtf(3.0) / 12.0 * (3.0 + sqrtf(5.0)));
        
        m_icosahedronMesh = PrimitiveMesh::makeIcosahedronMesn(m_device, icosahedronDescriptor, icoshedronRadius);
    }
    
    // Create a sphere for the skybox
    {
        m_skyMesh = PrimitiveMesh::makeSphereMesh(m_device, m_skyVertexDescriptor, 20, 20, 150.0);
    }
    
    // Load textures for non mesh assets
    {
        TextureLoader textureLoader(m_device);
        
        TextureLoaderOptions textureLoaderOptions;
        
        textureLoaderOptions.usage = MTL::TextureUsageShaderRead;
        textureLoaderOptions.storageMode = MTL::StorageModePrivate;
        
        const std::array<std::string, 6> images = {"X+.png", "X-.png", "Y+.png", "Y-.png", "Z+.png", "Z-.png"};
        m_skyMap = textureLoader.loadCubeTexture("../assets/SkyMap", images, true);
        
//        m_skyMap = textureLoader.makeTexture("SkyMap",
//                                             1.0,
//                                             textureLoaderOptions,
//                                             &error);
        
        AAPLAssert(error == nullptr, error, "Could not load sky texture");
        
        m_skyMap.label("Sky Map");
        
        m_fairyMap = textureLoader.loadTexture("../assets/Textures/", "fairy.png", true);
        
//        m_fairyMap = textureLoader.makeTexture("file://../assets/Textures/fairy.png",
//                                               textureLoaderOptions,
//                                               &error);
        
        AAPLAssert(error == nullptr, error, "Could not load fairy texture");
        
        m_fairyMap.label("Fairy Map");
    }
}

/// Initialize light positions and colors
void Subpass::populateLights() {
    PointLight *light_data = (PointLight *) m_lightsData.contents();
    
    m_originalLightPositions = new float4[NumLights];
    
    float4 *light_position = m_originalLightPositions;
    
    srandom(0x134e5348);
    
    for (uint32_t lightId = 0; lightId < NumLights; lightId++) {
        float distance = 0;
        float height = 0;
        float angle = 0;
        float speed = 0;
        
        if (lightId < TreeLights) {
            distance = random_float(38, 42);
            height = random_float(0, 1);
            angle = random_float(0, M_PI * 2);
            speed = random_float(0.003, 0.014);
        } else if (lightId < GroundLights) {
            distance = random_float(140, 260);
            height = random_float(140, 150);
            angle = random_float(0, M_PI * 2);
            speed = random_float(0.006, 0.027);
            speed *= (random() % 2) * 2 - 1;
        } else if (lightId < ColumnLights) {
            distance = random_float(365, 380);
            height = random_float(150, 190);
            angle = random_float(0, M_PI * 2);
            speed = random_float(0.004, 0.014);
            speed *= (random() % 2) * 2 - 1;
        }
        
        speed *= .5;
        *light_position = (float4) {distance * sinf(angle), height, distance * cosf(angle), 1};
        light_data->light_radius = random_float(25, 35) / 10.0;
        light_data->light_speed = speed;
        
        int colorId = random() % 3;
        if (colorId == 0) {
            light_data->light_color = (float3) {random_float(4, 6), random_float(0, 4), random_float(0, 4)};
        } else if (colorId == 1) {
            light_data->light_color = (float3) {random_float(0, 4), random_float(4, 6), random_float(0, 4)};
        } else {
            light_data->light_color = (float3) {random_float(0, 4), random_float(0, 4), random_float(4, 6)};
        }
        
        light_data++;
        light_position++;
    }
}

/// Update light positions
void Subpass::updateLights(const float4x4 &modelViewMatrix) {
    PointLight *lightData = (PointLight *) m_lightsData.contents();
    
    float4 *currentBuffer =
    (float4 *) m_lightPositions[m_frameDataBufferIndex].contents();
    
    float4 *originalLightPositions = (float4 *) m_originalLightPositions;
    
    for (int i = 0; i < NumLights; i++) {
        float4 currentPosition;
        
        if (i < TreeLights) {
            double lightPeriod = lightData[i].light_speed * m_frameNumber;
            lightPeriod += originalLightPositions[i].y;
            lightPeriod -= floor(lightPeriod);  // Get fractional part
            
            // Use pow to slowly move the light outward as it reaches the branches of the tree
            float r = 1.2 + 10.0 * powf(lightPeriod, 5.0);
            
            currentPosition.x = originalLightPositions[i].x * r;
            currentPosition.y = 200.0f + lightPeriod * 400.0f;
            currentPosition.z = originalLightPositions[i].z * r;
            currentPosition.w = 1;
        } else {
            float rotationRadians = lightData[i].light_speed * m_frameNumber;
            float4x4 rotation = matrix4x4_rotation(rotationRadians, 0, 1, 0);
            currentPosition = rotation * originalLightPositions[i];
        }
        
        currentPosition = modelViewMatrix * currentPosition;
        currentBuffer[i] = currentPosition;
    }
}

/// Update application state for the current frame
void Subpass::updateWorldState() {
    m_frameNumber++;
    m_frameDataBufferIndex = (m_frameDataBufferIndex + 1) % MaxFramesInFlight;
    
    FrameData *frameData = (FrameData *) (m_uniformBuffers[m_frameDataBufferIndex].contents());
    
    // Set projection matrix and calculate inverted projection matrix
    frameData->projection_matrix = m_projection_matrix;
    frameData->projection_matrix_inverse = matrix_invert(m_projection_matrix);
    
    // Set screen dimensions
    frameData->framebuffer_width = (uint) m_albedo_specular_GBuffer.width();
    frameData->framebuffer_height = (uint) m_albedo_specular_GBuffer.height();
    
    frameData->shininess_factor = 1;
    frameData->fairy_specular_intensity = 32;
    
    float cameraRotationRadians = m_frameNumber * 0.0025f + M_PI;
    
    float3 cameraRotationAxis = {0, 1, 0};
    float4x4 cameraRotationMatrix = matrix4x4_rotation(cameraRotationRadians, cameraRotationAxis);
    
    float4x4 view_matrix = matrix_look_at_left_hand(0, 18, -50,
                                                    0, 5, 0,
                                                    0, 1, 0);
    
    view_matrix = view_matrix * cameraRotationMatrix;
    
    frameData->view_matrix = view_matrix;
    
    float4x4 templeScaleMatrix = matrix4x4_scale(0.1, 0.1, 0.1);
    float4x4 templeTranslateMatrix = matrix4x4_translation(0, -10, 0);
    float4x4 templeModelMatrix = templeTranslateMatrix * templeScaleMatrix;
    frameData->temple_model_matrix = templeModelMatrix;
    frameData->temple_modelview_matrix = frameData->view_matrix * templeModelMatrix;
    frameData->temple_normal_matrix = matrix3x3_upper_left(frameData->temple_model_matrix);
    
    float skyRotation = m_frameNumber * 0.005f - (M_PI_4 * 3);
    
    float3 skyRotationAxis = {0, 1, 0};
    float4x4 skyModelMatrix = matrix4x4_rotation(skyRotation, skyRotationAxis);
    frameData->sky_modelview_matrix = cameraRotationMatrix * skyModelMatrix;
    
    // Update directional light color
    float4 sun_color = {0.5, 0.5, 0.5, 1.0};
    frameData->sun_color = sun_color;
    frameData->sun_specular_intensity = 1;
    
    // Update sun direction in view space
    float4 sunModelPosition = {-0.25, -0.5, 1.0, 0.0};
    
    float4 sunWorldPosition = skyModelMatrix * sunModelPosition;
    
    float4 sunWorldDirection = -sunWorldPosition;
    
    frameData->sun_eye_direction = view_matrix * sunWorldDirection;
    
    {
        float4 directionalLightUpVector = {0.0, 1.0, 1.0, 1.0};
        
        directionalLightUpVector = skyModelMatrix * directionalLightUpVector;
        directionalLightUpVector.xyz = normalize(directionalLightUpVector.xyz);
        
        float4x4 shadowViewMatrix = matrix_look_at_left_hand(sunWorldDirection.xyz / 10,
                                                             (float3) {0, 0, 0},
                                                             directionalLightUpVector.xyz);
        
        float4x4 shadowModelViewMatrix = shadowViewMatrix * templeModelMatrix;
        
        frameData->shadow_mvp_matrix = m_shadowProjectionMatrix * shadowModelViewMatrix;
    }
    
    {
        // When calculating texture coordinates to sample from shadow map, flip the y/t coordinate and
        // convert from the [-1, 1] range of clip coordinates to [0, 1] range of
        // used for texture sampling
        float4x4 shadowScale = matrix4x4_scale(0.5f, -0.5f, 1.0);
        float4x4 shadowTranslate = matrix4x4_translation(0.5, 0.5, 0);
        float4x4 shadowTransform = shadowTranslate * shadowScale;
        
        frameData->shadow_mvp_xform_matrix = shadowTransform * frameData->shadow_mvp_matrix;
    }
    
    frameData->fairy_size = .4;
    
    updateLights(frameData->temple_modelview_matrix);
}

/// Called whenever view changes orientation or layout is changed
void Subpass::drawableSizeWillChange(MTL::Size size, MTL::StorageMode GBufferStorageMode) {
    // When reshape is called, update the aspect ratio and projection matrix since the view
    //   orientation or size has changed
    float aspect = (float) size.width / (float) size.height;
    m_projection_matrix = matrix_perspective_left_hand(65.0f * (M_PI / 180.0f), aspect, NearPlane, FarPlane);
    
    MTL::TextureDescriptor GBufferTextureDesc;
    
    GBufferTextureDesc.pixelFormat(MTL::PixelFormatRGBA8Unorm_sRGB);
    GBufferTextureDesc.width(size.width);
    GBufferTextureDesc.height(size.height);
    GBufferTextureDesc.mipmapLevelCount(1);
    GBufferTextureDesc.textureType(MTL::TextureType2D);
    
    if (GBufferStorageMode == MTL::StorageModePrivate) {
        GBufferTextureDesc.usage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
    } else {
        GBufferTextureDesc.usage(MTL::TextureUsageRenderTarget);
    }
    
    GBufferTextureDesc.storageMode(GBufferStorageMode);
    
    GBufferTextureDesc.pixelFormat(m_albedo_specular_GBufferFormat);
    m_albedo_specular_GBuffer = m_device.makeTexture(GBufferTextureDesc);
    
    GBufferTextureDesc.pixelFormat(m_normal_shadow_GBufferFormat);
    m_normal_shadow_GBuffer = m_device.makeTexture(GBufferTextureDesc);
    
    GBufferTextureDesc.pixelFormat(m_depth_GBufferFormat);
    m_depth_GBuffer = m_device.makeTexture(GBufferTextureDesc);
    
    m_albedo_specular_GBuffer.label("Albedo + Shadow GBuffer");
    m_normal_shadow_GBuffer.label("Normal + Specular GBuffer");
    m_depth_GBuffer.label("Depth GBuffer");
}

#pragma mark Common Rendering Code

/// Draw the Mesh objects with the given renderEncoder
void Subpass::drawMeshes(MTL::RenderCommandEncoder &renderEncoder) {
    for (auto &mesh: *m_meshes) {
        for (auto &meshBuffer: mesh.vertexBuffers()) {
            renderEncoder.setVertexBuffer(meshBuffer.buffer(),
                                          meshBuffer.offset(),
                                          meshBuffer.argumentIndex());
        }
        
        for (auto &submesh: mesh.submeshes()) {
            // Set any textures read/sampled from the render pipeline
            const std::vector<MTL::Texture> &submeshTextures = submesh.textures();
            
            renderEncoder.setFragmentTexture(submeshTextures[TextureIndexBaseColor], TextureIndexBaseColor);
            
            renderEncoder.setFragmentTexture(submeshTextures[TextureIndexNormal], TextureIndexNormal);
            
            renderEncoder.setFragmentTexture(submeshTextures[TextureIndexSpecular], TextureIndexSpecular);
            
            renderEncoder.drawIndexedPrimitives(submesh.primitiveType(),
                                                submesh.indexCount(),
                                                submesh.indexType(),
                                                submesh.indexBuffer().buffer(),
                                                submesh.indexBuffer().offset());
        }
    }
}

/// Get a drawable from the view (or hand back an offscreen drawable for buffer examination mode)
MTL::Texture *Subpass::currentDrawableTexture() {
    MTL::Drawable *drawable = m_view->currentDrawable();
    
    if (drawable) {
        return drawable->texture();
    }
    
    return nullptr;
}

/// Perform operations necessary at the beginning of the frame.  Wait on the in flight semaphore,
/// and get a command buffer to encode intial commands for this frame.
MTL::CommandBuffer Subpass::beginFrame() {
    // Wait to ensure only MaxFramesInFlight are getting processed by any stage in the Metal
    // pipeline (App, Metal, Drivers, GPU, etc)
    
    dispatch_semaphore_wait(this->m_inFlightSemaphore, DISPATCH_TIME_FOREVER);
    
    // Create a new command buffer for each render pass to the current drawable
    MTL::CommandBuffer commandBuffer = m_commandQueue.commandBuffer();
    
    updateWorldState();
    
    return commandBuffer;
}

/// Perform operations necessary to obtain a command buffer for rendering to the drawable.  By
/// endoding commands that are not dependant on the drawable in a separate command buffer, Metal
/// can begin executing encoded commands for the frame (commands from the previous command buffer)
/// before a drawable for this frame becomes avaliable.
MTL::CommandBuffer Subpass::beginDrawableCommands() {
    MTL::CommandBuffer commandBuffer = m_commandQueue.commandBuffer();
    
    if (!m_completedHandler) {
        // Create a completed handler functor for Metal to execute when the GPU has fully finished
        // processing the commands encoded for this frame.  This implenentation of the completed
        // hander signals the `m_inFlightSemaphore`, which indicates that the GPU is no longer
        // accesing the the dynamic buffer written this frame.  When the GPU no longer accesses the
        // buffer, the Subpass can safely overwrite the buffer's data to update data for a future
        // frame.
        struct CommandBufferCompletedHandler : public MTL::CommandBufferHandler {
            dispatch_semaphore_t semaphore;
            
            void operator()(const MTL::CommandBuffer &) {
                dispatch_semaphore_signal(semaphore);
            }
        };
        
        CommandBufferCompletedHandler *completedHandler = new CommandBufferCompletedHandler();
        completedHandler->semaphore = m_inFlightSemaphore;
        
        m_completedHandler = completedHandler;
    }
    
    commandBuffer.addCompletedHandler(*m_completedHandler);
    
    return commandBuffer;
}

/// Perform cleanup operations including presenting the drawable and committing the command buffer
/// for the current frame.  Also, when enabled, draw buffer examination elements before all this.
void Subpass::endFrame(MTL::CommandBuffer &commandBuffer) {
    // Schedule a present once the framebuffer is complete using the current drawable
    if (m_view->currentDrawable()) {
        // Create a scheduled handler functor for Metal to present the drawable when the command
        // buffer has been scheduled by the kernel.
        struct PresentationScheduledHandler : public MTL::CommandBufferHandler {
            MTL::Drawable m_drawable;
            
            PresentationScheduledHandler(MTL::Drawable drawable)
            : m_drawable(drawable) {
            }
            
            void operator()(const MTL::CommandBuffer &) {
                m_drawable.present();
                delete this;
            }
        };
        
        PresentationScheduledHandler *scheduledHandler =
        new PresentationScheduledHandler(*m_view->currentDrawable());
        
        commandBuffer.addScheduledHandler(*scheduledHandler);
    }
    
    // Finalize rendering here & push the command buffer to the GPU
    commandBuffer.commit();
}

/// Draw to the depth texture from the directional lights point of view to generate the shadow map
void Subpass::drawShadow(MTL::CommandBuffer &commandBuffer) {
    MTL::RenderCommandEncoder encoder = commandBuffer.renderCommandEncoderWithDescriptor(m_shadowRenderPassDescriptor);
    
    encoder.label("Shadow Map Pass");
    
    encoder.setRenderPipelineState(m_shadowGenPipelineState);
    encoder.setDepthStencilState(m_shadowDepthStencilState);
    encoder.setCullMode(MTL::CullModeBack);
    encoder.setDepthBias(0.015, 7, 0.02);
    
    encoder.setVertexBuffer(m_uniformBuffers[m_frameDataBufferIndex], 0, BufferIndexFrameData);
    
    drawMeshes(encoder);
    
    encoder.endEncoding();
}

/// Draw to the three textures which compose the GBuffer
void Subpass::drawGBuffer(MTL::RenderCommandEncoder &renderEncoder) {
    renderEncoder.pushDebugGroup("Draw G-Buffer");
    renderEncoder.setCullMode(MTL::CullModeBack);
    renderEncoder.setRenderPipelineState(m_GBufferPipelineState);
    renderEncoder.setDepthStencilState(m_GBufferDepthStencilState);
    renderEncoder.setStencilReferenceValue(128);
    renderEncoder.setVertexBuffer(m_uniformBuffers[m_frameDataBufferIndex], 0, BufferIndexFrameData);
    renderEncoder.setFragmentBuffer(m_uniformBuffers[m_frameDataBufferIndex], 0, BufferIndexFrameData);
    renderEncoder.setFragmentTexture(m_shadowMap, TextureIndexShadow);
    
    drawMeshes(renderEncoder);
    renderEncoder.popDebugGroup();
}

/// Draw the directional ("sun") light in deferred pass.  Use stencil buffer to limit execution
/// of the shader to only those pixels that should be lit
void Subpass::drawDirectionalLightCommon(MTL::RenderCommandEncoder &renderEncoder) {
    renderEncoder.setCullMode(MTL::CullModeBack);
    renderEncoder.setStencilReferenceValue(128);
    
    renderEncoder.setRenderPipelineState(m_directionalLightPipelineState);
    renderEncoder.setDepthStencilState(m_directionLightDepthStencilState);
    renderEncoder.setVertexBuffer(m_quadVertexBuffer, 0, BufferIndexMeshPositions);
    renderEncoder.setVertexBuffer(m_uniformBuffers[m_frameDataBufferIndex], 0, BufferIndexFrameData);
    renderEncoder.setFragmentBuffer(m_uniformBuffers[m_frameDataBufferIndex], 0, BufferIndexFrameData);
    
    // Draw full screen quad
    renderEncoder.drawPrimitives(MTL::PrimitiveTypeTriangle, 0, 6);
}

/// Render to stencil buffer only to increment stencil on that fragments in front
/// of the backside of each light volume
void Subpass::drawPointLightMask(MTL::RenderCommandEncoder &renderEncoder) {
#if LIGHT_STENCIL_CULLING
    renderEncoder.pushDebugGroup("Draw Light Mask");
    renderEncoder.setRenderPipelineState(m_lightMaskPipelineState);
    renderEncoder.setDepthStencilState(m_lightMaskDepthStencilState);
    
    renderEncoder.setStencilReferenceValue(128);
    renderEncoder.setCullMode(MTL::CullModeFront);
    
    renderEncoder.setVertexBuffer(m_uniformBuffers[m_frameDataBufferIndex], 0, BufferIndexFrameData);
    renderEncoder.setFragmentBuffer(m_uniformBuffers[m_frameDataBufferIndex], 0, BufferIndexFrameData);
    renderEncoder.setVertexBuffer(m_lightsData, 0, BufferIndexLightsData);
    renderEncoder.setVertexBuffer(m_lightPositions[m_frameDataBufferIndex], 0, BufferIndexLightsPosition);
    
    const std::vector<MeshBuffer> &vertexBuffers = m_icosahedronMesh.vertexBuffers();
    renderEncoder.setVertexBuffer(vertexBuffers[0].buffer(), vertexBuffers[0].offset(), BufferIndexMeshPositions);
    
    const std::vector<Submesh> &icosahedronSubmesh = m_icosahedronMesh.submeshes();
    
    renderEncoder.drawIndexedPrimitives(icosahedronSubmesh[0].primitiveType(),
                                        icosahedronSubmesh[0].indexCount(),
                                        icosahedronSubmesh[0].indexType(),
                                        icosahedronSubmesh[0].indexBuffer().buffer(),
                                        icosahedronSubmesh[0].indexBuffer().offset(),
                                        NumLights);
    
    renderEncoder.popDebugGroup();
#endif
}

/// Performs operations common to both single-pass and traditional deferred renders for drawing point lights.
/// Called by derived subpass classes  after they have set up any subpass specific specific state
/// (such as setting GBuffer textures with the traditional deferred subpass not needed for the single-pass subpass)
void Subpass::drawPointLightsCommon(MTL::RenderCommandEncoder &renderEncoder) {
    renderEncoder.setDepthStencilState(m_pointLightDepthStencilState);
    
    renderEncoder.setStencilReferenceValue(128);
    renderEncoder.setCullMode(MTL::CullModeBack);
    
    renderEncoder.setVertexBuffer(m_uniformBuffers[m_frameDataBufferIndex], 0, BufferIndexFrameData);
    renderEncoder.setVertexBuffer(m_lightsData, 0, BufferIndexLightsData);
    renderEncoder.setVertexBuffer(m_lightPositions[m_frameDataBufferIndex], 0, BufferIndexLightsPosition);
    
    renderEncoder.setFragmentBuffer(m_uniformBuffers[m_frameDataBufferIndex], 0, BufferIndexFrameData);
    renderEncoder.setFragmentBuffer(m_lightsData, 0, BufferIndexLightsData);
    renderEncoder.setFragmentBuffer(m_lightPositions[m_frameDataBufferIndex], 0, BufferIndexLightsPosition);
    
    const std::vector<MeshBuffer> &vertexBuffers = m_icosahedronMesh.vertexBuffers();
    renderEncoder.setVertexBuffer(vertexBuffers[0].buffer(), vertexBuffers[0].offset(), BufferIndexMeshPositions);
    
    const std::vector<Submesh> &icosahedronSubmesh = m_icosahedronMesh.submeshes();
    
    renderEncoder.drawIndexedPrimitives(icosahedronSubmesh[0].primitiveType(),
                                        icosahedronSubmesh[0].indexCount(),
                                        icosahedronSubmesh[0].indexType(),
                                        icosahedronSubmesh[0].indexBuffer().buffer(),
                                        icosahedronSubmesh[0].indexBuffer().offset(),
                                        NumLights);
}

/// Draw the "fairies" at the center of the point lights with a 2D disk using a texture to perform
/// smooth alpha blending on the edges
void Subpass::drawFairies(MTL::RenderCommandEncoder &renderEncoder) {
    renderEncoder.pushDebugGroup("Draw Fairies");
    renderEncoder.setRenderPipelineState(m_fairyPipelineState);
    renderEncoder.setDepthStencilState(*m_dontWriteDepthStencilState);
    renderEncoder.setCullMode(MTL::CullModeBack);
    renderEncoder.setVertexBuffer(m_uniformBuffers[m_frameDataBufferIndex], 0, BufferIndexFrameData);
    renderEncoder.setVertexBuffer(m_fairy, 0, BufferIndexMeshPositions);
    renderEncoder.setVertexBuffer(m_lightsData, 0, BufferIndexLightsData);
    renderEncoder.setVertexBuffer(m_lightPositions[m_frameDataBufferIndex], 0, BufferIndexLightsPosition);
    renderEncoder.setFragmentTexture(m_fairyMap, TextureIndexAlpha);
    renderEncoder.drawPrimitives(MTL::PrimitiveTypeTriangleStrip, 0, NumFairyVertices, NumLights);
    renderEncoder.popDebugGroup();
}

/// Draw the sky dome behind all other geometry (testing against depth buffer generated in
///  GBuffer pass)
void Subpass::drawSky(MTL::RenderCommandEncoder &renderEncoder) {
    renderEncoder.pushDebugGroup("Draw Sky");
    renderEncoder.setRenderPipelineState(m_skyboxPipelineState);
    renderEncoder.setDepthStencilState(*m_dontWriteDepthStencilState);
    renderEncoder.setCullMode(MTL::CullModeFront);
    
    renderEncoder.setVertexBuffer(m_uniformBuffers[m_frameDataBufferIndex], 0, BufferIndexFrameData);
    renderEncoder.setFragmentTexture(m_skyMap, TextureIndexBaseColor);
    
    for (auto &meshBuffer: m_skyMesh.vertexBuffers()) {
        renderEncoder.setVertexBuffer(meshBuffer.buffer(),
                                      meshBuffer.offset(),
                                      meshBuffer.argumentIndex());
    }
    
    
    for (auto &submesh: m_skyMesh.submeshes()) {
        renderEncoder.drawIndexedPrimitives(submesh.primitiveType(),
                                            submesh.indexCount(),
                                            submesh.indexType(),
                                            submesh.indexBuffer().buffer(),
                                            submesh.indexBuffer().offset());
    }
    renderEncoder.popDebugGroup();
}


MTL::Library Subpass::makeShaderLibrary() {
    CFErrorRef error = nullptr;
    CFURLRef libraryURL = nullptr;

    libraryURL = CFBundleCopyResourceURL( CFBundleGetMainBundle() , CFSTR("vox.shader"), CFSTR("metallib"), nullptr);
    MTL::Library shaderLibrary = m_device.makeLibrary(libraryURL, &error);
    
    AAPLAssert(!error, error, "Could not load Metal shader library");
    
    CFRelease(libraryURL);
    return shaderLibrary;
}

}
