//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "particle.h"
#include "entity.h"
#include "scene.h"
#include "mesh/mesh_renderer.h"
#include "particle_manager.h"
#include "metal_helpers.h"

namespace vox {
namespace {
unsigned int closestPowerOfTwo(unsigned int const n) {
    unsigned int r = 1u;
    for (unsigned int i = 0u; r < n; r <<= 1u)
        ++i;
    return r;
}

unsigned int numTrailingBits(unsigned int const n) {
    unsigned int r = 0u;
    for (unsigned int i = n; i > 1u; i >>= 1u)
        ++r;
    return r;
}

}  // namespace

Particle::Particle(Entity *entity) :
Script(entity),
_timeStepProp(Shader::createProperty("uTimeStep", ShaderDataGroup::Compute)),
_vectorFieldTextureProp(Shader::createProperty("uVectorFieldTexture", ShaderDataGroup::Compute)),
_boundingVolumeProp(Shader::createProperty("uBoundingVolume", ShaderDataGroup::Compute)),
_bboxSizeProp(Shader::createProperty("uBBoxSize", ShaderDataGroup::Compute)),
_scatteringFactorProp(Shader::createProperty("uScatteringFactor", ShaderDataGroup::Compute)),
_vectorFieldFactorProp(Shader::createProperty("uVectorFieldFactor", ShaderDataGroup::Compute)),
_curlNoiseFactorProp(Shader::createProperty("uCurlNoiseFactor", ShaderDataGroup::Compute)),
_curlNoiseScaleProp(Shader::createProperty("uCurlNoiseScale", ShaderDataGroup::Compute)),
_velocityFactorProp(Shader::createProperty("uVelocityFactor", ShaderDataGroup::Compute)),

_emitCountProp(Shader::createProperty("uEmitCount", ShaderDataGroup::Compute)),
_emitterTypeProp(Shader::createProperty("uEmitterType", ShaderDataGroup::Compute)),
_emitterPositionProp(Shader::createProperty("uEmitterPosition", ShaderDataGroup::Compute)),
_emitterDirectionProp(Shader::createProperty("uEmitterDirection", ShaderDataGroup::Compute)),
_emitterRadiusProp(Shader::createProperty("uEmitterRadius", ShaderDataGroup::Compute)),
_particleMinAgeProp(Shader::createProperty("uParticleMinAge", ShaderDataGroup::Compute)),
_particleMaxAgeProp(Shader::createProperty("uParticleMaxAge", ShaderDataGroup::Compute)) {
    setParticleMinAge(50.f);
    setParticleMaxAge(100.f);
    setEmitterType(EmitterType::SPHERE);
    setEmitterDirection(Vector3F(0, 1, 0));
    setEmitterRadius(32.0f);
    setBoundingVolumeType(SimulationVolume::SPHERE);
    setBBoxSize(kDefaultSimulationVolumeSize);
    
    setScatteringFactor(1.f);
    setVectorFieldFactor(1.f);
    setCurlNoiseFactor(16.f);
    setCurlNoiseScale(128.f);
    setVelocityFactor(8.f);
    
    _allocBuffer();
    
    _renderer = entity->addComponent<MeshRenderer>();
    _material = std::make_shared<ParticleMaterial>();
    _renderer->setMaterial(_material);
}

void Particle::_allocBuffer() {
    auto& device = entity()->scene()->device();
    
    /* Assert than the number of particles will be a factor of threadGroupWidth */
    unsigned int const num_particles = floorParticleCount(kMaxParticleCount); //
    fprintf(stderr, "[ %u particles, %u per batch ]\n", num_particles , kBatchEmitCount);
    
    /* Random value buffer */
    unsigned int const num_randvalues = 3u * num_particles;
    _randomVec.resize(num_randvalues);
    _randomBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(sizeof(float) * num_randvalues,
                                                                             MTL::ResourceOptionCPUCacheModeDefault));
    
    /* Atomic */
    uint32_t atomicInit = 0;
    _atomicBuffer[0] = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(&atomicInit, sizeof(uint32_t),
                                                                                MTL::ResourceOptionCPUCacheModeDefault));
    _atomicBuffer[1] = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(&atomicInit, sizeof(uint32_t),
                                                                                MTL::ResourceOptionCPUCacheModeDefault));
    
    /* Append Consume */
#if USE_SOA_LAYOUT
    _positionBuffer[0] = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(sizeof(Vector4F) * num_particles,
                                                                                  MTL::ResourceOptionCPUCacheModeDefault));
    _positionBuffer[1] = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(sizeof(Vector4F) * num_particles,
                                                                                  MTL::ResourceOptionCPUCacheModeDefault));
    
    _velocityBuffer[0] = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(sizeof(Vector4F) * num_particles,
                                                                                  MTL::ResourceOptionCPUCacheModeDefault));
    _velocityBuffer[1] = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(sizeof(Vector4F) * num_particles,
                                                                                  MTL::ResourceOptionCPUCacheModeDefault));
    
    _attributeBuffer[0] = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(sizeof(Vector4F) * num_particles,
                                                                                   MTL::ResourceOptionCPUCacheModeDefault));
    _attributeBuffer[1] = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(sizeof(Vector4F) * num_particles,
                                                                                   MTL::ResourceOptionCPUCacheModeDefault));
#else
    _appendConsumeBuffer[0] = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(sizeof(TParticle) * num_particles,
                                                                                       MTL::ResourceOptionCPUCacheModeDefault));
    _appendConsumeBuffer[1] = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(sizeof(TParticle) * num_particles,
                                                                                       MTL::ResourceOptionCPUCacheModeDefault));
#endif
    
    /* Sort buffers */
    // The parallel nature of the sorting algorithm needs power of two sized buffer.
    unsigned int const sort_buffer_max_count = closestPowerOfTwo(kMaxParticleCount); //
    _dpBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(sizeof(float) * sort_buffer_max_count,
                                                                         MTL::ResourceOptionCPUCacheModeDefault));
    _sortIndicesBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(sizeof(uint32_t) * sort_buffer_max_count * 2,
                                                                                  MTL::ResourceOptionCPUCacheModeDefault));
}

void Particle::_allocMesh() {
    for (int i = 0; i < 2; i++) {
        auto vertexDescriptor = CLONE_METAL_CUSTOM_DELETER(MTL::VertexDescriptor, MTL::VertexDescriptor::alloc()->init());
#if USE_SOA_LAYOUT
        vertexDescriptor->attributes()->object(0)->setFormat(MTL::VertexFormat::VertexFormatFloat3);
        vertexDescriptor->attributes()->object(0)->setOffset(0);
        vertexDescriptor->attributes()->object(0)->setBufferIndex(0);
        vertexDescriptor->attributes()->object(1)->setFormat(MTL::VertexFormat::VertexFormatFloat3);
        vertexDescriptor->attributes()->object(1)->setOffset(0);
        vertexDescriptor->attributes()->object(1)->setBufferIndex(1);
        vertexDescriptor->attributes()->object(2)->setFormat(MTL::VertexFormat::VertexFormatFloat2);
        vertexDescriptor->attributes()->object(2)->setOffset(0);
        vertexDescriptor->attributes()->object(2)->setBufferIndex(2);
        vertexDescriptor->layouts()->object(0)->setStride(sizeof(Vector4F));
        vertexDescriptor->layouts()->object(1)->setStride(sizeof(Vector4F));
        vertexDescriptor->layouts()->object(2)->setStride(sizeof(Vector4F));
#else
        vertexDescriptor->attributes()->object(0)->setFormat(MTL::VertexFormat::VertexFormatFloat3);
        vertexDescriptor->attributes()->object(0)->setOffset(0);
        vertexDescriptor->attributes()->object(0)->setBufferIndex(0);
        vertexDescriptor->attributes()->object(1)->setFormat(MTL::VertexFormat::VertexFormatFloat3);
        vertexDescriptor->attributes()->object(1)->setOffset(sizeof(Vector4F));
        vertexDescriptor->attributes()->object(1)->setBufferIndex(0);
        vertexDescriptor->attributes()->object(2)->setFormat(MTL::VertexFormat::VertexFormatFloat2);
        vertexDescriptor->attributes()->object(2)->setOffset(2 * sizeof(Vector4F));
        vertexDescriptor->attributes()->object(2)->setBufferIndex(0);
        vertexDescriptor->layouts()->object(0)->setStride(sizeof(TParticle));
#endif
        
        auto bufferMesh = std::make_shared<BufferMesh>();
        bufferMesh->setVertexLayouts(vertexDescriptor);
        bufferMesh->addSubMesh(MTL::PrimitiveType::PrimitiveTypePoint, MTL::IndexType::IndexTypeUInt16, 0, nullptr);
#if USE_SOA_LAYOUT
        bufferMesh->setVertexBufferBinding(_positionBuffer[i], 0);
        bufferMesh->setVertexBufferBinding(_positionBuffer[i], 1);
        bufferMesh->setVertexBufferBinding(_positionBuffer[i], 2);
#else
        bufferMesh->setVertexBufferBinding(_appendConsumeBuffer[i], 0);
#endif
        _meshes[i] = bufferMesh;
    }
}

void Particle::_generateRandomValues() {
    std::uniform_real_distribution<float> distrib(_minValue, _maxValue);
    for (unsigned int i = 0u; i < _randomBuffer->length() / sizeof(float); ++i) {
        _randomVec[i] = distrib(_mt);
    }
    memcpy(_randomBuffer->contents(), _randomVec.data(), _randomBuffer->length());
}

void Particle::onUpdate(float deltaTime) {
    _write = 1 - _write;
    _read = 1 - _read;
    
    _meshes[_write]->subMesh()->setIndexCount(0);
    _renderer->setMesh(_meshes[_write]);
    _generateRandomValues();
}

ParticleMaterial& Particle::material() {
    return *_material;
}

float Particle::timeStep() const {
    return std::any_cast<float>(shaderData.getData(_timeStepProp));
}

void Particle::setTimeStep(float step) {
    shaderData.setData(_timeStepProp, step);
}

Particle::SimulationVolume Particle::boundingVolumeType() const {
    return (Particle::SimulationVolume)std::any_cast<uint32_t>(shaderData.getData(_boundingVolumeProp));
}

void Particle::setBoundingVolumeType(SimulationVolume type) {
    shaderData.setData(_boundingVolumeProp, (uint32_t)type);
}

float Particle::bboxSize() const {
    return std::any_cast<float>(shaderData.getData(_bboxSizeProp));
}

void Particle::setBBoxSize(float size) {
    shaderData.setData(_bboxSizeProp, size);
}

float Particle::scatteringFactor() const {
    return std::any_cast<float>(shaderData.getData(_scatteringFactorProp));
}

void Particle::setScatteringFactor(float factor) {
    shaderData.enableMacro(NEED_PARTICLE_SCATTERING);
    shaderData.setData(_scatteringFactorProp, factor);
}

std::shared_ptr<SampledTexture3D> Particle::vectorFieldTexture() const {
    return std::any_cast<std::shared_ptr<SampledTexture3D>>(shaderData.getData(_vectorFieldTextureProp));
}

void Particle::setVectorFieldTexture(const std::shared_ptr<SampledTexture3D>& field) {
    shaderData.enableMacro(NEED_PARTICLE_VECTOR_FIELD);
    shaderData.setSampledTexure(_vectorFieldTextureProp, field);
}

float Particle::vectorFieldFactor() const {
    return std::any_cast<float>(shaderData.getData(_vectorFieldFactorProp));
}

void Particle::setVectorFieldFactor(float factor) {
    shaderData.setData(_vectorFieldFactorProp, factor);
}

float Particle::curlNoiseFactor() const {
    return std::any_cast<float>(shaderData.getData(_curlNoiseFactorProp));
}

void Particle::setCurlNoiseFactor(float factor) {
    shaderData.enableMacro(NEED_PARTICLE_CURL_NOISE);
    shaderData.setData(_curlNoiseFactorProp, factor);
}

float Particle::curlNoiseScale() const {
    return std::any_cast<float>(shaderData.getData(_curlNoiseScaleProp));
}

void Particle::setCurlNoiseScale(float scale) {
    shaderData.enableMacro(NEED_PARTICLE_CURL_NOISE);
    shaderData.setData(_curlNoiseScaleProp, scale);
}

float Particle::velocityFactor() const {
    return std::any_cast<float>(shaderData.getData(_velocityFactorProp));
}

void Particle::setVelocityFactor(float factor) {
    shaderData.enableMacro(NEED_PARTICLE_VELOCITY_CONTROL);
    shaderData.setData(_velocityFactorProp, factor);
}

//MARK: - Emitter
uint32_t Particle::emitCount() const {
    return std::any_cast<uint32_t>(shaderData.getData(_emitCountProp));
}

void Particle::setEmitCount(uint32_t count) {
    shaderData.setData(_emitCountProp, count);
}

Particle::EmitterType Particle::emitterType() const {
    return (Particle::EmitterType)std::any_cast<uint32_t>(shaderData.getData(_emitterTypeProp));
}

void Particle::setEmitterType(EmitterType type) {
    shaderData.setData(_emitterTypeProp, (uint32_t)type);
}

Vector3F Particle::emitterPosition() const {
    return std::any_cast<Vector3F>(shaderData.getData(_emitterPositionProp));
}

void Particle::setEmitterPosition(const Vector3F& position) {
    shaderData.setData(_emitterPositionProp, position);
}

Vector3F Particle::emitterDirection() const {
    return std::any_cast<Vector3F>(shaderData.getData(_emitterDirectionProp));
}

void Particle::setEmitterDirection(const Vector3F& direction) {
    shaderData.setData(_emitterDirectionProp, direction);
}

float Particle::emitterRadius() const {
    return std::any_cast<float>(shaderData.getData(_emitterRadiusProp));
}

void Particle::setEmitterRadius(float radius) {
    shaderData.setData(_emitterRadiusProp, radius);
}

float Particle::particleMinAge() const {
    return std::any_cast<float>(shaderData.getData(_particleMinAgeProp));
}

void Particle::setParticleMinAge(float age) {
    shaderData.setData(_particleMinAgeProp, age);
}

float Particle::particleMaxAge() const {
    return std::any_cast<float>(shaderData.getData(_particleMaxAgeProp));
}

void Particle::setParticleMaxAge(float age) {
    shaderData.setData(_particleMaxAgeProp, age);
}

void Particle::_onEnable() {
    entity()->scene()->_particleManager.addParticle(this);
}

void Particle::_onDisable() {
    entity()->scene()->_particleManager.removeParticle(this);
}

}
