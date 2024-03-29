//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_manager_hpp
#define particle_manager_hpp

#include "particle_renderer.h"
#include "rendering/compute_pass.h"
#include "singleton.h"

namespace vox {
class ParticleManager : public Singleton<ParticleManager> {
public:
    static ParticleManager &getSingleton(void);
    
    static ParticleManager *getSingletonPtr(void);
    
    ParticleManager(MTL::Library &library, Scene *scene);
    
    const std::vector<ParticleRenderer*>& particles() const;
    
    void addParticle(ParticleRenderer* particle);
    
    void removeParticle(ParticleRenderer* particle);
        
    void draw(MTL::CommandBuffer& commandBuffer);
    
public:
    float timeStepFactor() const;

    void setTimeStepFactor(float factor);
    
private:
    void _computeSingle(ParticleRenderer* particle, MTL::ComputeCommandEncoder &commandEncoder,
                        const ShaderMacroCollection &compileMacros);
    
    void _emission(const uint32_t count,
                   ParticleRenderer* particle, MTL::ComputeCommandEncoder &commandEncoder,
                   const ShaderMacroCollection &compileMacros);
    
    void _simulation(const uint32_t count,
                     ParticleRenderer* particle, MTL::ComputeCommandEncoder &commandEncoder,
                     const ShaderMacroCollection &compileMacros);
    
private:
    std::vector<ParticleRenderer*> _particles{};
    float _timeStepFactor = 1.0f;
    
    std::unique_ptr<ComputePass> _emissionPass{nullptr};
    std::unique_ptr<ComputePass> _simulationPass{nullptr};
};

template<> inline ParticleManager* Singleton<ParticleManager>::msSingleton{nullptr};

}
#endif /* particle_manager_hpp */
