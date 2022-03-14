//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "particle_renderer.h"
#include "entity.h"
#include "scene.h"
#include "mesh/mesh_renderer.h"
#include "particle_manager.h"
#include "metal_helpers.h"

namespace vox {
namespace {
uint32_t closestPowerOfTwo(uint32_t const n) {
    uint32_t r = 1u;
    for (uint32_t i = 0u; r < n; r <<= 1u)
        ++i;
    return r;
}

}  // namespace

uint32_t const ParticleRenderer::kBatchEmitCount;

ParticleRenderer::ParticleRenderer(Entity *entity) :
Renderer(entity),
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
    _allocBuffer();
    _allocMesh();
    
    _renderer = entity->addComponent<MeshRenderer>();
    _material = std::make_shared<ParticleMaterial>();
    _renderer->setMaterial(_material);
}

void ParticleRenderer::_allocBuffer() {
    auto& device = entity()->scene()->device();
    
    /* Assert than the number of particles will be a factor of threadGroupWidth */
    fprintf(stderr, "[ %u particles, %u per batch ]\n", kMaxParticleCount , kBatchEmitCount);
    
    /* Random value buffer */
    uint32_t const num_randvalues = 3u * kMaxParticleCount;
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
    _appendConsumeBuffer[0] = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(sizeof(TParticle) * kMaxParticleCount,
                                                                                       MTL::ResourceOptionCPUCacheModeDefault));
    _appendConsumeBuffer[1] = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(sizeof(TParticle) * kMaxParticleCount,
                                                                                       MTL::ResourceOptionCPUCacheModeDefault));
    
    /* Sort buffers */
    // The parallel nature of the sorting algorithm needs power of two sized buffer.
    unsigned int const sort_buffer_max_count = closestPowerOfTwo(kMaxParticleCount); //
    _dpBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(sizeof(float) * sort_buffer_max_count,
                                                                         MTL::ResourceOptionCPUCacheModeDefault));
    _sortIndicesBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(sizeof(uint32_t) * sort_buffer_max_count * 2,
                                                                                  MTL::ResourceOptionCPUCacheModeDefault));
}

void ParticleRenderer::_allocMesh() {
    for (int i = 0; i < 2; i++) {
        auto vertexDescriptor = CLONE_METAL_CUSTOM_DELETER(MTL::VertexDescriptor, MTL::VertexDescriptor::alloc()->init());
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
        
        auto bufferMesh = std::make_shared<BufferMesh>();
        bufferMesh->setVertexLayouts(vertexDescriptor);
        bufferMesh->addSubMesh(MTL::PrimitiveType::PrimitiveTypePoint, MTL::IndexType::IndexTypeUInt16, 0, nullptr);
        bufferMesh->setVertexBufferBinding(_appendConsumeBuffer[i], 0);
        _meshes[i] = bufferMesh;
    }
}

void ParticleRenderer::_generateRandomValues() {
    std::uniform_real_distribution<float> distrib(_minValue, _maxValue);
    for (unsigned int i = 0u; i < _randomBuffer->length() / sizeof(float); ++i) {
        _randomVec[i] = distrib(_mt);
    }
    memcpy(_randomBuffer->contents(), _randomVec.data(), _randomBuffer->length());
}

void ParticleRenderer::update(float deltaTime) {
    setTimeStep(deltaTime * ParticleManager::getSingleton().timeStepFactor());
    
    _write = 1 - _write;
    _read = 1 - _read;
    
    memcpy(&_numAliveParticles, _atomicBuffer[_read]->contents(), sizeof(uint32_t));
    _meshes[_read]->setInstanceCount(_numAliveParticles);
    _renderer->setMesh(_meshes[_read]);
    _generateRandomValues();
}

ParticleMaterial& ParticleRenderer::material() {
    return *_material;
}

float ParticleRenderer::timeStep() const {
    return std::any_cast<float>(shaderData.getData(_timeStepProp));
}

void ParticleRenderer::setTimeStep(float step) {
    shaderData.setData(_timeStepProp, step);
}

ParticleRenderer::SimulationVolume ParticleRenderer::boundingVolumeType() const {
    return (ParticleRenderer::SimulationVolume)std::any_cast<uint32_t>(shaderData.getData(_boundingVolumeProp));
}

void ParticleRenderer::setBoundingVolumeType(SimulationVolume type) {
    shaderData.setData(_boundingVolumeProp, (uint32_t)type);
}

float ParticleRenderer::bboxSize() const {
    return std::any_cast<float>(shaderData.getData(_bboxSizeProp));
}

void ParticleRenderer::setBBoxSize(float size) {
    shaderData.setData(_bboxSizeProp, size);
}

float ParticleRenderer::scatteringFactor() const {
    return std::any_cast<float>(shaderData.getData(_scatteringFactorProp));
}

void ParticleRenderer::setScatteringFactor(float factor) {
    shaderData.enableMacro(NEED_PARTICLE_SCATTERING);
    shaderData.setData(_scatteringFactorProp, factor);
}

std::shared_ptr<SampledTexture3D> ParticleRenderer::vectorFieldTexture() const {
    return std::any_cast<std::shared_ptr<SampledTexture3D>>(shaderData.getData(_vectorFieldTextureProp));
}

void ParticleRenderer::setVectorFieldTexture(const std::shared_ptr<SampledTexture3D>& field) {
    shaderData.enableMacro(NEED_PARTICLE_VECTOR_FIELD);
    shaderData.setSampledTexure(_vectorFieldTextureProp, field);
}

float ParticleRenderer::vectorFieldFactor() const {
    return std::any_cast<float>(shaderData.getData(_vectorFieldFactorProp));
}

void ParticleRenderer::setVectorFieldFactor(float factor) {
    shaderData.setData(_vectorFieldFactorProp, factor);
}

float ParticleRenderer::curlNoiseFactor() const {
    return std::any_cast<float>(shaderData.getData(_curlNoiseFactorProp));
}

void ParticleRenderer::setCurlNoiseFactor(float factor) {
    shaderData.enableMacro(NEED_PARTICLE_CURL_NOISE);
    shaderData.setData(_curlNoiseFactorProp, factor);
}

float ParticleRenderer::curlNoiseScale() const {
    return std::any_cast<float>(shaderData.getData(_curlNoiseScaleProp));
}

void ParticleRenderer::setCurlNoiseScale(float scale) {
    shaderData.enableMacro(NEED_PARTICLE_CURL_NOISE);
    shaderData.setData(_curlNoiseScaleProp, scale);
}

float ParticleRenderer::velocityFactor() const {
    return std::any_cast<float>(shaderData.getData(_velocityFactorProp));
}

void ParticleRenderer::setVelocityFactor(float factor) {
    shaderData.enableMacro(NEED_PARTICLE_VELOCITY_CONTROL);
    shaderData.setData(_velocityFactorProp, factor);
}

//MARK: - Emitter
uint32_t ParticleRenderer::emitCount() const {
    return std::any_cast<uint32_t>(shaderData.getData(_emitCountProp));
}

void ParticleRenderer::setEmitCount(uint32_t count) {
    shaderData.setData(_emitCountProp, count);
}

ParticleRenderer::EmitterType ParticleRenderer::emitterType() const {
    return (ParticleRenderer::EmitterType)std::any_cast<uint32_t>(shaderData.getData(_emitterTypeProp));
}

void ParticleRenderer::setEmitterType(EmitterType type) {
    shaderData.setData(_emitterTypeProp, (uint32_t)type);
}

Vector3F ParticleRenderer::emitterPosition() const {
    return std::any_cast<Vector3F>(shaderData.getData(_emitterPositionProp));
}

void ParticleRenderer::setEmitterPosition(const Vector3F& position) {
    shaderData.setData(_emitterPositionProp, position);
}

Vector3F ParticleRenderer::emitterDirection() const {
    return std::any_cast<Vector3F>(shaderData.getData(_emitterDirectionProp));
}

void ParticleRenderer::setEmitterDirection(const Vector3F& direction) {
    shaderData.setData(_emitterDirectionProp, direction);
}

float ParticleRenderer::emitterRadius() const {
    return std::any_cast<float>(shaderData.getData(_emitterRadiusProp));
}

void ParticleRenderer::setEmitterRadius(float radius) {
    shaderData.setData(_emitterRadiusProp, radius);
}

float ParticleRenderer::particleMinAge() const {
    return std::any_cast<float>(shaderData.getData(_particleMinAgeProp));
}

void ParticleRenderer::setParticleMinAge(float age) {
    shaderData.setData(_particleMinAgeProp, age);
}

float ParticleRenderer::particleMaxAge() const {
    return std::any_cast<float>(shaderData.getData(_particleMaxAgeProp));
}

void ParticleRenderer::setParticleMaxAge(float age) {
    shaderData.setData(_particleMaxAgeProp, age);
}

void ParticleRenderer::_onEnable() {
    Renderer::_onEnable();
    ParticleManager::getSingleton().addParticle(this);
}

void ParticleRenderer::_onDisable() {
    Renderer::_onDisable();
    ParticleManager::getSingleton().removeParticle(this);
}

//MARK: - Buffer
const uint32_t ParticleRenderer::numAliveParticles() const {
    return _numAliveParticles;
}

const std::shared_ptr<MTL::Buffer>& ParticleRenderer::randomBuffer() const {
    return _randomBuffer;
}

const std::shared_ptr<MTL::Buffer>& ParticleRenderer::readAtomicBuffer() const {
    return _atomicBuffer[_read];
}

const std::shared_ptr<MTL::Buffer>& ParticleRenderer::writeAtomicBuffer() const {
    return _atomicBuffer[_write];
}

const std::shared_ptr<MTL::Buffer>& ParticleRenderer::readAppendConsumeBuffer() const {
    return _appendConsumeBuffer[_read];
}

const std::shared_ptr<MTL::Buffer>& ParticleRenderer::writeAppendConsumeBuffer() const {
    return _appendConsumeBuffer[_write];
}

const std::shared_ptr<MTL::Buffer>& ParticleRenderer::dpBuffer() const {
    return _dpBuffer;
}

const std::shared_ptr<MTL::Buffer>& ParticleRenderer::sortIndicesBuffer() const {
    return _sortIndicesBuffer;
}

}
