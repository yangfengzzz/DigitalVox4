//
//  editor.cpp
//  editor
//
//  Created by 杨丰 on 2022/1/21.
//

#include "editor.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/unlit_material.h"
#include "material/blinn_phong_material.h"
#include "camera.h"
#include "physics/static_collider.h"
#include "physics/dynamic_collider.h"
#include "physics/shape/box_collider_shape.h"
#include "physics/shape/sphere_collider_shape.h"
#include "lighting/point_light.h"
#include "controls/orbit_control.h"

#include "grid.h"
#include <random>

namespace vox {
namespace {
class MoveScript : public Script {
    Point3F pos = Point3F(-5, 0, 0);
    float vel = 4;
    int8_t velSign = -1;
    
public:
    MoveScript(Entity *entity) : Script(entity) {
    }
    
    void onUpdate(float deltaTime) override {
        if (pos.x >= 5) {
            velSign = -1;
        }
        if (pos.x <= -5) {
            velSign = 1;
        }
        pos.x += deltaTime * vel * float(velSign);
        
        entity()->transform->setPosition(pos);
    }
};

// Collision Detection
class CollisionScript : public Script {
    MeshRenderer *sphereRenderer;
    std::default_random_engine e;
    std::uniform_real_distribution<float> u;
    
public:
    CollisionScript(Entity *entity) : Script(entity) {
        sphereRenderer = entity->getComponent<MeshRenderer>();
        u = std::uniform_real_distribution<float>(0, 1);
    }
    
    void onTriggerExit(physics::ColliderShapePtr other) override {
        static_cast<UnlitMaterial *>(sphereRenderer->getMaterial().get())->setBaseColor(Color(u(e), u(e), u(e), 1));
    }
    
    void onTriggerEnter(physics::ColliderShapePtr other) override {
        static_cast<UnlitMaterial *>(sphereRenderer->getMaterial().get())->setBaseColor(Color(u(e), u(e), u(e), 1));
    }
};
} // namespace

void Editor::loadScene(uint32_t width, uint32_t height) {
    _scene->ambientLight().setDiffuseSolidColor(Color(1, 1, 1));
    
    auto rootEntity = _scene->createRootEntity();
    rootEntity->addComponent<editor::Grid>();

    auto cameraEntity = rootEntity->createChild("camera");
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    _mainCamera->resize(width, height);
    cameraEntity->addComponent<control::OrbitControl>();
    _entry = cameraEntity->addComponent<editor::GUIEntry>();

    // init point light
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(0, 3, 0);
    auto pointLight = light->addComponent<PointLight>();
    pointLight->intensity = 0.3;
    
    // create box test entity
    float cubeSize = 2.0;
    auto boxEntity = rootEntity->createChild("BoxEntity");
    auto boxMtl = std::make_shared<BlinnPhongMaterial>();
    auto boxRenderer = boxEntity->addComponent<MeshRenderer>();
    boxMtl->setBaseColor(Color(0.8, 0.3, 0.3, 1.0));
    boxRenderer->setMesh(PrimitiveMesh::createCuboid(_device.get(), cubeSize, cubeSize, cubeSize));
    boxRenderer->setMaterial(boxMtl);
    
    auto boxCollider = boxEntity->addComponent<physics::StaticCollider>();
    auto boxColliderShape = std::make_shared<physics::BoxColliderShape>();
    boxColliderShape->setSize(Vector3F(cubeSize, cubeSize, cubeSize));
    boxCollider->addShape(boxColliderShape);
    
    // create sphere test entity
    float radius = 1.25;
    auto sphereEntity = rootEntity->createChild("SphereEntity");
    sphereEntity->transform->setPosition(Point3F(-5, 0, 0));
    auto sphereRenderer = sphereEntity->addComponent<MeshRenderer>();
    auto sphereMtl = std::make_shared<BlinnPhongMaterial>();
    std::default_random_engine e;
    std::uniform_real_distribution<float> u(0, 1);
    sphereMtl->setBaseColor(Color(u(e), u(e), u(e), 1));
    sphereRenderer->setMesh(PrimitiveMesh::createSphere(_device.get(), radius));
    sphereRenderer->setMaterial(sphereMtl);
    
    auto sphereCollider = sphereEntity->addComponent<physics::DynamicCollider>();
    auto sphereColliderShape = std::make_shared<physics::SphereColliderShape>();
    sphereColliderShape->setRadius(radius);
    sphereColliderShape->setTrigger(true);
    sphereCollider->addShape(sphereColliderShape);
    
    sphereEntity->addComponent<CollisionScript>();
    sphereEntity->addComponent<MoveScript>();
}

void Editor::pickFunctor(Renderer *renderer, MeshPtr mesh) {
    if (renderer && mesh) {
        _entry->setRenderer(renderer);
    }
}

void Editor::inputEvent(const InputEvent &inputEvent) {
    EditorApplication::inputEvent(inputEvent);
    
    if (inputEvent.source() == EventSource::Mouse) {
        const auto &mouse_button = static_cast<const MouseButtonInputEvent &>(inputEvent);
        if (mouse_button.action() == MouseAction::Down) {
            pick(mouse_button.pos_x(), mouse_button.pos_y());
        }
    }
}

}
