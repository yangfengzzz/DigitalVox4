//
//  skybox_app.cpp
//  apps
//
//  Created by 杨丰 on 2022/1/22.
//

#include "skybox_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/unlit_material.h"
#include "rendering/subpasses/skybox_subpass.h"
#include "loader/texture_loader.h"
#include "camera.h"

namespace vox {
bool SkyboxApp::prepare(Engine &engine) {
    ForwardApplication::prepare(engine);
    
    auto resourceLoader = TextureLoader(*_device);
    
    auto skybox = std::make_unique<SkyboxSubpass>(_renderView.get(), _scene.get(), _mainCamera);
    skybox->createCuboid();
    skybox->setTextureCubeMap(resourceLoader.loadTexture("/Users/yangfeng/Desktop/Vulkan/data/textures",
                                                         "cubemap_yokohama_rgba.ktx"));
    _renderPass->addSubpass(std::move(skybox));
    
    return true;
}

void SkyboxApp::loadScene(uint32_t width, uint32_t height) {
    auto rootEntity = _scene->createRootEntity();
    
    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    _mainCamera->resize(width, height);
    cameraEntity->addComponent<control::OrbitControl>();
    
    auto modelEntity = rootEntity->createChild();
    auto renderer = modelEntity->addComponent<MeshRenderer>();
    renderer->setMesh(PrimitiveMesh::createCuboid(_device.get()));
    auto material = std::make_shared<UnlitMaterial>();
    material->setBaseColor(Color(0.6, 0.4, 0.7, 1.0));
    renderer->setMaterial(material);
}

}
