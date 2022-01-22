//
//  ibl_app.cpp
//  apps
//
//  Created by 杨丰 on 2022/1/22.
//

#include "ibl_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/pbr_material.h"
#include "rendering/subpasses/skybox_subpass.h"
#include "loader/texture_loader.h"
#include "camera.h"

namespace vox {
bool IBLApp::prepare(Engine &engine) {
    ForwardApplication::prepare(engine);
    
    auto resourceLoader = TextureLoader(*_device);
    
    auto skybox = std::make_unique<SkyboxSubpass>(_renderView.get(), _scene.get(), _mainCamera);
    skybox->createCuboid();
    skybox->setTextureCubeMap(resourceLoader.loadCubeTexture(_path, _images));
    _renderPass->addSubpass(std::move(skybox));
    
    return true;
}

void IBLApp::loadScene(uint32_t width, uint32_t height) {
    _materials.resize(11);
    _materials[0] = Material("Gold", Color(1.0f, 0.765557f, 0.336057f, 1.0), 0.1f, 1.0f);
    _materials[1] = Material("Copper", Color(0.955008f, 0.637427f, 0.538163f, 1.0), 0.1f, 1.0f);
    _materials[2] = Material("Chromium", Color(0.549585f, 0.556114f, 0.554256f, 1.0), 0.1f, 1.0f);
    _materials[3] = Material("Nickel", Color(0.659777f, 0.608679f, 0.525649f, 1.0), 0.1f, 1.0f);
    _materials[4] = Material("Titanium", Color(0.541931f, 0.496791f, 0.449419f, 1.0), 0.1f, 1.0f);
    _materials[5] = Material("Cobalt", Color(0.662124f, 0.654864f, 0.633732f, 1.0), 0.1f, 1.0f);
    _materials[6] = Material("Platinum", Color(0.672411f, 0.637331f, 0.585456f, 1.0), 0.1f, 1.0f);
    // Testing materials
    _materials[7] = Material("White", Color(1.0f, 1.0, 1.0, 1.0), 0.1f, 1.0f);
    _materials[8] = Material("Red", Color(1.0f, 0.0f, 0.0f, 1.0), 0.1f, 1.0f);
    _materials[9] = Material("Blue", Color(0.0f, 0.0f, 1.0f, 1.0), 0.1f, 1.0f);
    _materials[10] = Material("Black", Color(0.0f, 1.0, 1.0, 1.0), 0.1f, 1.0f);
    
    const int materialIndex = 0;
    Material mat = _materials[materialIndex];
    
    _path = "../assets/SkyMap/country";
    _images = {"posx.png", "negx.png", "posy.png", "negy.png", "posz.png", "negz.png"};
    
    auto resourceLoader = TextureLoader(*_device);
    auto specularTexture = resourceLoader.createSpecularTexture(_path, _images);
    _scene->ambientLight().setSpecularTexture(specularTexture);
    
    auto diffuseTexture = resourceLoader.createIrradianceTexture(_path, _images);
    _scene->ambientLight().setDiffuseTexture(diffuseTexture);
    
    auto rootEntity = _scene->createRootEntity();
    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    _mainCamera->resize(width, height);
    cameraEntity->addComponent<control::OrbitControl>();
    
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(-3, 3, -3);
    light->transform->lookAt(Point3F(0, 0, 0));
    auto directionLight = light->addComponent<DirectLight>();
    directionLight->intensity = 0.3;
    
    auto sphere = PrimitiveMesh::createSphere(_device.get(), 0.5, 64);
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            auto sphereEntity = rootEntity->createChild("SphereEntity" + std::to_string(i) + std::to_string(j));
            sphereEntity->transform->setPosition(Point3F(i - 3, j - 3, 0));
            auto sphereMtl = std::make_shared<PBRMaterial>();
            sphereMtl->setBaseColor(mat.baseColor);
            sphereMtl->setMetallic(clamp(float(7 - i) / float(7 - 1), 0.1f, 1.0f));
            sphereMtl->setRoughness(clamp(float(7 - j) / float(7 - 1), 0.05f, 1.0f));
            
            auto sphereRenderer = sphereEntity->addComponent<MeshRenderer>();
            sphereRenderer->setMesh(sphere);
            sphereRenderer->setMaterial(sphereMtl);
        }
    }
}

}
