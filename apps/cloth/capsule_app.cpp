//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "capsule_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/blinn_phong_material.h"
#include "camera.h"
#include "controls/orbit_control.h"

#include "cloth/cloth_mesh_generator.h"

namespace vox {
namespace cloth {
using namespace physx;

void CapsuleApp::_initializeCloth(EntityPtr entity, physx::PxVec3 offset) {
    _solver = _factory->createSolver();
    trackSolver(_solver);
    
    ClothMeshData clothMesh;
    physx::PxMat44 transform = PxTransform(PxVec3(0.f, 13.f, 0.f) + offset, PxQuat(0, PxVec3(1.f, 0.f, 0.f)));
    clothMesh.GeneratePlaneCloth(5.f, 6.f, 69, 79, false, transform);
    clothMesh.AttachClothPlaneByAngles(69, 79);
    clothMesh.SetInvMasses(0.5f);
    
    nv::cloth::ClothMeshDesc meshDesc = clothMesh.GetClothMeshDesc();
    nv::cloth::Vector<int32_t>::Type phaseTypeInfo;
    _fabric = NvClothCookFabricFromMesh(_factory, meshDesc, physx::PxVec3(0.0f, 0.0f, 1.0f), &phaseTypeInfo, false);
    trackFabric(_fabric);
    
    _clothActor.clothRenderer = entity->addComponent<ClothRenderer>();
    _clothActor.clothRenderer->setClothMeshDesc(meshDesc);
    _clothActor.clothRenderer->setMaterial(std::make_shared<BlinnPhongMaterial>());
    
    // Initialize start positions and masses for the actual cloth instance
    // (note: the particle/vertex positions do not have to match the mesh description here. Set the positions to the initial shape of this cloth instance)
    std::vector<physx::PxVec4> particlesCopy;
    particlesCopy.resize(clothMesh.mVertices.size());
    physx::PxVec3 clothOffset = transform.getPosition();
    for (int i = 0; i < (int) clothMesh.mVertices.size(); i++) {
        // To put attachment point closer to each other
        if (clothMesh.mInvMasses[i] < 1e-6)
            clothMesh.mVertices[i] = (clothMesh.mVertices[i] - clothOffset) * 0.95f + clothOffset;
        
        particlesCopy[i] = physx::PxVec4(clothMesh.mVertices[i], clothMesh.mInvMasses[i]); // w component is 1/mass, or 0.0f for anchored/fixed particles
    }
    // Create the cloth from the initial positions/masses and the fabric
    _clothActor.cloth = _factory->createCloth(nv::cloth::Range<physx::PxVec4>(&particlesCopy[0], &particlesCopy[0] + particlesCopy.size()), *_fabric);
    _clothActor.cloth->setGravity(physx::PxVec3(0.0f, -9.8f, 0.0f));
    _clothActor.cloth->setDamping(physx::PxVec3(0.1f, 0.1f, 0.1f));
    physx::PxVec4 spheres[2] = {physx::PxVec4(physx::PxVec3(0.f, 10.f, -2.f) + offset, 3.0),
        physx::PxVec4(physx::PxVec3(0.f, 10.f, 2.f) + offset, 1.0)};
    _clothActor.cloth->setSpheres(nv::cloth::Range<physx::PxVec4>(spheres, spheres + 2), 0, _clothActor.cloth->getNumSpheres());
    
    uint32_t caps[4];
    caps[0] = 0;
    caps[1] = 1;
    _clothActor.cloth->setCapsules(nv::cloth::Range<uint32_t>(caps, caps + 2), 0, _clothActor.cloth->getNumCapsules());
    
    // Setup phase configs
    std::vector<nv::cloth::PhaseConfig> phases(_fabric->getNumPhases());
    for (int i = 0; i < (int) phases.size(); i++) {
        phases[i].mPhaseIndex = i;
        phases[i].mStiffness = 1.0f;
        phases[i].mStiffnessMultiplier = 1.0f;
        phases[i].mCompressionLimit = 1.0f;
        phases[i].mStretchLimit = 1.0f;
    }
    _clothActor.cloth->setPhaseConfig(nv::cloth::Range<nv::cloth::PhaseConfig>(&phases.front(), &phases.back()));
    _clothActor.cloth->setDragCoefficient(0.5f);
    _clothActor.cloth->setLiftCoefficient(0.6f);
    trackClothActor(&_clothActor);
    
    // Add the cloth to the solver for simulation
    addClothToSolver(&_clothActor, _solver);
}

void CapsuleApp::loadScene(uint32_t width, uint32_t height) {
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
    
    auto modelEntity = rootEntity->createChild();
    auto renderer = modelEntity->addComponent<MeshRenderer>();
    renderer->setMesh(PrimitiveMesh::createCuboid(_device.get()));
    auto material = std::make_shared<BlinnPhongMaterial>();
    material->setBaseColor(Color(0.6, 0.4, 0.7, 1.0));
    renderer->setMaterial(material);
    
    auto clothEntity = rootEntity->createChild();
    _initializeCloth(clothEntity, physx::PxVec3(0.0f, 0.0f, 0.0f));
}

}
}
