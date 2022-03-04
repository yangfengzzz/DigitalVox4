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

void ParticleApp::loadScene(uint32_t width, uint32_t height) {    
    auto rootEntity = _scene->createRootEntity();
    
    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    _mainCamera->resize(width, height);
    cameraEntity->addComponent<control::OrbitControl>();
    
    // init point light
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(0, 3, 0);
    auto pointLight = light->addComponent<PointLight>();
    pointLight->intensity = 0.3;
    
    auto cubeEntity = rootEntity->createChild();
    auto renderer = cubeEntity->addComponent<MeshRenderer>();
    renderer->setMesh(PrimitiveMesh::createPlane(*_device, 10, 10));
    auto material = std::make_shared<BlinnPhongMaterial>();
    material->setBaseColor(Color(0.4, 0.6, 0.6));
    renderer->setMaterial(material);
}


}
