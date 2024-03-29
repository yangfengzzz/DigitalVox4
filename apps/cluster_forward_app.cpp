//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cluster_forward_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "camera.h"
#include "controls/orbit_control.h"
#include "lighting/point_light.h"
#include "lighting/cluster_debug_material.h"
#include "lighting/sprite/sprite_debug.h"
#include <random>

namespace vox {
void ClusterForwardApp::loadScene() {
    _scene->ambientLight().setDiffuseSolidColor(Color(1, 1, 1));
    
    auto rootEntity = _scene->createRootEntity();
    rootEntity->addComponent<SpriteDebug>();
    
    auto cameraEntity = rootEntity->createChild("camera");
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0, 1.0);
    // init point light
    for (uint32_t i = 0; i < 15; i++) {
        auto light = rootEntity->createChild("light");
        light->transform->setPosition(10 * dis(gen), 2, 10 * dis(gen));
        light->addComponent<PointLight>();
    }
    
    // init spot light
    for (uint32_t i = 0; i < 15; i++) {
        auto light = rootEntity->createChild("light");
        light->transform->setPosition(10 * dis(gen), 2, 10 * dis(gen));
        light->addComponent<SpotLight>();
    }
    
    // init point light
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(3, 3, 0);
    light->addComponent<PointLight>();
    
    auto light2 = rootEntity->createChild("light");
    light2->transform->setPosition(-3, 3, 0);
    light2->addComponent<SpotLight>();
    
    // create box test entity
    float cubeSize = 20.0;
    auto boxEntity = rootEntity->createChild("BoxEntity");
    auto boxMtl = std::make_shared<ClusterDebugMaterial>();
    auto boxRenderer = boxEntity->addComponent<MeshRenderer>();
    boxRenderer->setMesh(PrimitiveMesh::createPlane(*_device, cubeSize, cubeSize, 100, 1000));
    boxRenderer->setMaterial(boxMtl);
}

}
