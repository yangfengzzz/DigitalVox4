//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_hpp
#define particle_hpp

#include "component.h"
#include "particle_material.h"
#include "texture/sampled_texture3d.h"

namespace vox {
class Particle : public Component {
public:
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
    
    explicit Particle(Entity *entity);
    
    ParticleMaterial& material();
    
public:
    float timeStep() const;
    
    void setTimeStep(float step);
    
    std::shared_ptr<SampledTexture3D> vectorFieldTexture() const;
    
    void setVectorFieldTexture(const std::shared_ptr<SampledTexture3D>& field);
    
    SimulationVolume boundingVolumeType() const;
    
    void setBoundingVolumeType(SimulationVolume vol);
    
    float bboxSize() const;
    
    void setBBoxSize(float size);
    
    float scatteringFactor() const;
    
    void setScatteringFactor(float factor);
    
    float vectorFieldFactor() const;
    
    void setVectorFieldFactor(float factor);
    
    float curlNoiseFactor() const;
    
    void setCurlNoiseFactor(float factor);
    
    float curlNoiseScale() const;
    
    void setCurlNoiseScale(float scale);
    
    float velocityFactor() const;
    
    void setVelocityFactor(float factor);
    
    bool enableScattering() const;
    
    void setEnableScattering(bool flag);
    
    bool enableVectorField() const;
    
    void setEnableVectorField(bool flag);
    
    bool enableCurlNoise() const;
    
    void setEnableCurlNoise(bool flag);
    
    bool enableVelocityControl() const;
    
    void setEnableVelocityControl(bool flag);
    
public:
    uint32_t emitCount() const;
    
    void setEmitCount(uint32_t count);
    
    EmitterType emitterType() const;
    
    void setEmitterType(EmitterType type);
    
    const Vector3F& emitterPosition() const;
    
    void setEmitterPosition(const Vector3F& position);
    
    const Vector3F& emitterDirection() const;
    
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
    
private:
    std::shared_ptr<ParticleMaterial> _material{nullptr};
    std::shared_ptr<SampledTexture3D> _field{nullptr};
    
    float _timeStep;
    SimulationVolume _boundingVolume;
    float _bboxSize;
    float _scatteringFactor;
    float _vectorFieldFactor;
    float _curlNoiseFactor;
    float _curlNoiseScale;
    float _velocityFactor;
    bool _enableScattering;
    bool _enableVectorField;
    bool _enableCurlNoise;
    bool _enableVelocityControl;
    
    uint32_t _emitCount;
    EmitterType _emitterType;
    Vector3F _emitterPosition;
    Vector3F _emitterDirection;
    float _emitterRadius;
    float _particleMinAge;
    float _particleMaxAge;
};
}

#endif /* particle_hpp */
