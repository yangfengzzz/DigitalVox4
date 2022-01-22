//
//  animation_app.cpp
//  apps
//
//  Created by 杨丰 on 2022/1/21.
//

#include "animation_app.h"
#include "camera.h"
#include "mesh/mesh_renderer.h"
#include "mesh/skinned_mesh_renderer.h"
#include "mesh/primitive_mesh.h"
#include "animator.h"
#include "material/unlit_material.h"
#include "material/blinn_phong_material.h"
#include "material/pbr_material.h"
#include "controls/orbit_control.h"
#include "lighting/direct_light.h"
#include "lighting/point_light.h"
#include "lighting/spot_light.h"
#include "loader/texture_loader.h"

namespace vox {
void AnimationApp::loadScene(uint32_t width, uint32_t height) {
    auto rootEntity = _scene->createRootEntity();
    
    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    _mainCamera->resize(width, height);
    cameraEntity->addComponent<control::OrbitControl>();
    
    // init point light
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(0, 10, 0);
    light->transform->lookAt(Point3F(0, 0, 0), Vector3F(1, 0, 0));
    auto spotLight = light->addComponent<SpotLight>();
    spotLight->intensity = 0.8;
    spotLight->setShadowIntensity(0.05);
    spotLight->setEnableShadow(true);
    
    auto light2 = rootEntity->createChild("light3");
    auto directLight = light2->addComponent<DirectLight>();
    directLight->intensity = 0.2;
    directLight->setShadowIntensity(0.2);
    directLight->setEnableShadow(true);
    
    auto resourceLoader = TextureLoader(*_device);
    auto characterMtl = std::make_shared<PBRMaterial>();
    characterMtl->setBaseTexture(resourceLoader.loadTexture("../assets/models/Doggy", "T_Doggy_1_diffuse.png", false));
    characterMtl->setOcclusionTexture(resourceLoader.loadTexture("../assets/models/Doggy", "T_Doggy_1_ao.png", false));
    characterMtl->setNormalTexture(resourceLoader.loadTexture("../assets/models/Doggy", "T_Doggy_normal.png", false));
    characterMtl->setMetallicRoughnessTexture(resourceLoader.createMetallicRoughnessTexture("../assets/models/Doggy", "T_Doggy_metallic.png",
                                                                                            "T_Doggy_roughness.png", false));
    
    auto characterEntity = rootEntity->createChild("characterEntity");
    characterEntity->transform->setScale(3, 3, 3);
    auto characterRenderer = characterEntity->addComponent<SkinnedMeshRenderer>();
    characterRenderer->castShadow = true;
    characterRenderer->addSkinnedMesh("../assets/models/Doggy/Doggy.fbx",
                                      "../assets/models/Doggy/doggy_skeleton.ozz");
    characterRenderer->setMaterial(characterMtl);
    auto characterAnim = characterEntity->addComponent<Animator>();
    characterAnim->addAnimationClip("../assets/models/Doggy/Run.ozz");
    
    auto planeEntity = rootEntity->createChild("PlaneEntity");
    auto planeMtl = std::make_shared<BlinnPhongMaterial>();
    planeMtl->setBaseColor(Color(1.0, 0, 0, 1.0));
    planeMtl->setRenderFace(RenderFace::Double);
    
    auto planeRenderer = planeEntity->addComponent<MeshRenderer>();
    planeRenderer->setMesh(PrimitiveMesh::createPlane(_device.get(), 10, 10));
    planeRenderer->setMaterial(planeMtl);
    planeRenderer->receiveShadow = true;
}

}
