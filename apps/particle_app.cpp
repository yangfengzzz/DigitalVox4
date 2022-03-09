//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "particle_app.h"
#include "entity.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/unlit_material.h"
#include "material/blinn_phong_material.h"
#include "camera.h"
#include "controls/orbit_control.h"
#include "image/stb.h"
#include "particle/particle_compute_subpass.h"
#include "metal_helpers.h"

namespace vox {
bool ParticleApp::prepare(Engine &engine) {
    ForwardApplication::prepare(engine);
    
    auto subpass = std::make_unique<ParticleComputeSubpass>(_renderContext.get(), _scene.get(), _mainCamera);
    _renderPass->addSubpass(std::move(subpass));
    
    return true;
}

void ParticleApp::loadScene() {
    _scene->background.solidColor = Color(0, 0, 0, 1);
    auto rootEntity = _scene->createRootEntity();
    
    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(-30, 30, 30);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();
    
    // init point light
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(0, 3, 0);
    auto pointLight = light->addComponent<PointLight>();
    pointLight->intensity = 0.3;
    
    auto particleEntity = rootEntity->createChild();
    auto particle = particleEntity->addComponent<Particle>();
    // emitter
    particle->setParticleMinAge(50.f);
    particle->setParticleMaxAge(100.f);
    particle->setEmitterType(Particle::EmitterType::SPHERE);
    particle->setEmitterDirection(Vector3F(0, 1, 0));
    particle->setEmitterPosition(Vector3F()); // todo
    particle->setEmitterRadius(2.0f);
    particle->setBoundingVolumeType(Particle::SimulationVolume::SPHERE);
    particle->setBBoxSize(Particle::kDefaultSimulationVolumeSize);
    
    // simulation
    particle->setScatteringFactor(1.f);
    particle->setVectorFieldFactor(1.f);
    particle->setCurlNoiseFactor(16.f);
    particle->setCurlNoiseScale(128.f);
    particle->setVelocityFactor(8.f);
    
    // material
    particle->material().setBirthGradient(Vector3F(0, 1, 0));
    particle->material().setDeathGradient(Vector3F(1, 0, 0));
    particle->material().setMinParticleSize(0.75f);
    particle->material().setMaxParticleSize(4.0f);
    particle->material().setFadeCoefficient(0.35f);
    particle->material().setDebugDraw(false);
    
    auto cubeEntity = rootEntity->createChild();
    auto renderer = cubeEntity->addComponent<MeshRenderer>();
    renderer->setMesh(PrimitiveMesh::createPlane(*_device, 10, 10));
    auto material = std::make_shared<BlinnPhongMaterial>();
    material->setBaseColor(Color(0.4, 0.6, 0.6));
    renderer->setMaterial(material);
}


}
