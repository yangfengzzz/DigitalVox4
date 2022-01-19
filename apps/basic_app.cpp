//
//  basic_apps.cpp
//  apps
//
//  Created by 杨丰 on 2022/1/19.
//

#include "basic_app.h"
#include "graphics/mesh_loader.h"
#include "camera.h"

namespace vox {
void BasicApp::loadScene(uint32_t width, uint32_t height) {
    auto rootEntity = _scene->createRootEntity();
    auto modelEntity = rootEntity->createChild();
    
    MeshLoader loader(_device.get());
    loader.loadMesh("../assets/Models", "Temple.obj", modelEntity);
    modelEntity->transform->setPosition(0, 10, 0);
    modelEntity->transform->setScale(0.1, 0.1, 0.1);
    
    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(-6.02535057, 36.6681671, 48.6991844);
    _mainCamera = cameraEntity->addComponent<Camera>();
    _mainCamera->resize(width, height);
    _controller = cameraEntity->addComponent<control::OrbitControl>();
}

void BasicApp::inputEvent(const InputEvent &inputEvent) {
    _controller->inputEvent(inputEvent);
}

}
