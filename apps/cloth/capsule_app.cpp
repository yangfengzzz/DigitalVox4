//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "capsule_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/unlit_material.h"
#include "camera.h"
#include "controls/orbit_control.h"

#include "cloth/cloth_mesh_generator.h"

namespace vox {
namespace cloth {
using namespace physx;

void CapsuleApp::_initializeCloth(physx::PxVec3 offset) {
    ClothMeshData clothMesh;

    physx::PxMat44 transform = PxTransform(PxVec3(0.f, 13.f, 0.f)+ offset, PxQuat(0, PxVec3(1.f, 0.f, 0.f)));
    clothMesh.GeneratePlaneCloth(5.f, 6.f, 69, 79, false, transform);
    clothMesh.AttachClothPlaneByAngles(69, 79);
    clothMesh.SetInvMasses(0.5f);
}

void CapsuleApp::loadScene(uint32_t width, uint32_t height) {
    auto rootEntity = _scene->createRootEntity();
    auto modelEntity = rootEntity->createChild();
    
    auto renderer = modelEntity->addComponent<MeshRenderer>();
    renderer->setMesh(PrimitiveMesh::createCuboid(_device.get()));
    auto material = std::make_shared<UnlitMaterial>();
    material->setBaseColor(Color(0.6, 0.4, 0.7, 1.0));
    renderer->setMaterial(material);
    
    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    _mainCamera->resize(width, height);
    cameraEntity->addComponent<control::OrbitControl>();
}

}
}
