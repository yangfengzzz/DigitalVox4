//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_hpp
#define particle_hpp

#include "renderer.h"
#include "particle_material.h"
#include "texture/sampled_texture3d.h"
#include "mesh/buffer_mesh.h"
#include "particle_config.h"
#include <random>

namespace vox {
class ParticleRenderer : public Renderer {
public:
    static float constexpr kDefaultSimulationVolumeSize = 32.0f;
    
    // [USER DEFINED]
    static uint32_t const kMaxParticleCount = (1u << 10u);
    static uint32_t const kBatchEmitCount = std::max(256u, (kMaxParticleCount >> 4u));
    
    enum class EmitterType : uint32_t {
        POINT,
        DISK,
        SPHERE,
        BALL,
        kNumEmitterType
    };
    
    enum class SimulationVolume : uint32_t {
        SPHERE,
        BOX,
        NONE,
        kNumSimulationVolume
    };
    
    explicit ParticleRenderer(Entity *entity);
    
    ParticleMaterial& material();
    
    const uint32_t numAliveParticles() const;
    
public:
    void _render(std::vector<RenderElement> &opaqueQueue,
                 std::vector<RenderElement> &alphaTestQueue,
                 std::vector<RenderElement> &transparentQueue) override;
    
    void _updateBounds(BoundingBox3F &worldBounds) override;
    
    void update(float deltaTime) override;
    
public:
    float timeStep() const;
    
    void setTimeStep(float step);
    
    SimulationVolume boundingVolumeType() const;
    
    void setBoundingVolumeType(SimulationVolume vol);
    
    float bboxSize() const;
    
    void setBBoxSize(float size);
    
    float scatteringFactor() const;
    
    void setScatteringFactor(float factor);
    
    float vectorFieldFactor() const;
    
    void setVectorFieldFactor(float factor);
    
    std::shared_ptr<SampledTexture3D> vectorFieldTexture() const;
    
    void setVectorFieldTexture(const std::shared_ptr<SampledTexture3D>& field);
    
    float curlNoiseFactor() const;
    
    void setCurlNoiseFactor(float factor);
    
    float curlNoiseScale() const;
    
    void setCurlNoiseScale(float scale);
    
    float velocityFactor() const;
    
    void setVelocityFactor(float factor);
    
public:
    uint32_t emitCount() const;
    
    void setEmitCount(uint32_t count);
    
    EmitterType emitterType() const;
    
    void setEmitterType(EmitterType type);
    
    Vector3F emitterPosition() const;
    
    void setEmitterPosition(const Vector3F& position);
    
    Vector3F emitterDirection() const;
    
    void setEmitterDirection(const Vector3F& direction);
    
    float emitterRadius() const;
    
    void setEmitterRadius(float radius);
    
    float particleMinAge() const;
    
    void setParticleMinAge(float age);
    
    float particleMaxAge() const;
    
    void setParticleMaxAge(float age);
    
private:
    void _onEnable() override;
    
    void _onDisable() override;
    
    void _allocBuffer();
        
    void _generateRandomValues();
    
private:
    uint32_t _numAliveParticles = 0;
    
    std::shared_ptr<BufferMesh> _mesh{nullptr};
    std::shared_ptr<ParticleMaterial> _material{nullptr};
    
    std::random_device _randomDevice;
    std::mt19937 _mt;
    float _minValue = 0.0;
    float _maxValue = 1.0;
    std::vector<float> _randomVec{};
    std::shared_ptr<MTL::Buffer> _randomBuffer{nullptr};
    ShaderProperty _randomBufferProp;
    
    uint32_t _read = 0;
    uint32_t _write = 1;
    std::shared_ptr<MTL::Buffer> _atomicBuffer[2];
    ShaderProperty _readCounterProp;
    ShaderProperty _writeCounterProp;
    
    std::shared_ptr<MTL::Buffer> _appendConsumeBuffer[2];
    ShaderProperty _readParticleProp;
    ShaderProperty _writeParticleProp;
    
    std::shared_ptr<MTL::Buffer> _dpBuffer{nullptr};
    ShaderProperty _dpBufferProp;
    std::shared_ptr<MTL::Buffer> _sortIndicesBuffer{nullptr};
    ShaderProperty _sortIndicesBufferProp;
    
    ShaderProperty _timeStepProp;
    ShaderProperty _vectorFieldTextureProp;
    ShaderProperty _boundingVolumeProp;
    ShaderProperty _bboxSizeProp;
    ShaderProperty _scatteringFactorProp;
    ShaderProperty _vectorFieldFactorProp;
    ShaderProperty _curlNoiseFactorProp;
    ShaderProperty _curlNoiseScaleProp;
    ShaderProperty _velocityFactorProp;
    
    ShaderProperty _emitCountProp;
    ShaderProperty _emitterTypeProp;
    ShaderProperty _emitterPositionProp;
    ShaderProperty _emitterDirectionProp;
    ShaderProperty _emitterRadiusProp;
    ShaderProperty _particleMinAgeProp;
    ShaderProperty _particleMaxAgeProp;
};
}

#endif /* particle_hpp */
