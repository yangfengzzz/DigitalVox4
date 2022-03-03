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
    explicit Particle(Entity *entity);
    
    ParticleMaterial& material();
    
public:
    float timeStep() const;
    
    void setTimeStep(float step);
    
    std::shared_ptr<SampledTexture3D> vectorFieldTexture() const;
    
    void setVectorFieldTexture(const std::shared_ptr<SampledTexture3D>& field);
    
    int boundingVolume() const;
    
    void setBoundingVolume(int vol);
    
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
    
    void setEnableVectorField(bool flag) const;
    
    bool enableCurlNoise() const;
    
    void setEnableCurlNoise(bool flag) const;
    
    bool enableVelocityControl() const;
    
    bool setEnableVelocityControl(bool flag) const;
    
private:
    std::shared_ptr<ParticleMaterial> _material{nullptr};
};
}

#endif /* particle_hpp */
