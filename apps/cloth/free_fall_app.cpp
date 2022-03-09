//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "free_fall_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/blinn_phong_material.h"
#include "camera.h"
#include "controls/orbit_control.h"

#include "cloth/cloth_mesh_generator.h"
#include "cloth/simple_mesh_utils.h"

namespace vox {
namespace cloth {
using namespace physx;

void FreeFallApp::_initializeCloth(EntityPtr entity, int index, physx::PxVec3 offset) {
    _solver = _factory->createSolver();
    trackSolver(_solver);
    
    ClothMeshData clothMesh;
    physx::PxMat44 transform = PxTransform(PxVec3(0.f, 13.f, 0.f)+ offset, PxQuat(PxPi, PxVec3(1.f, 0.f, 0.f)));
    clothMesh.GeneratePlaneCloth(1.f * (1 << index), 1.f * (1 << index), int(2.0f*(1 << index)), int(2.0f*(1 << index)), false, transform);

    nv::cloth::ClothMeshDesc meshDesc = clothMesh.GetClothMeshDesc();
    nv::cloth::Vector<int32_t>::Type phaseTypeInfo;
    _fabric[index] = NvClothCookFabricFromMesh(_factory, meshDesc, physx::PxVec3(0.0f, 0.0f, 1.0f), &phaseTypeInfo, false);
    trackFabric(_fabric[index]);
    
    _clothActor[index].clothRenderer = entity->addComponent<ClothRenderer>();
    _clothActor[index].clothRenderer->setClothMeshDesc(meshDesc);
    auto material = std::make_shared<BlinnPhongMaterial>();
    material->setRenderFace(RenderFace::Double);
    material->setBaseColor(Color(247/256.0, 186/256.0, 11/256.0));
    _clothActor[index].clothRenderer->setMaterial(material);
    
    // Initialize start positions and masses for the actual cloth instance
    // (note: the particle/vertex positions do not have to match the mesh description here. Set the positions to the initial shape of this cloth instance)
    std::vector<physx::PxVec4> particlesCopy;
    particlesCopy.resize(clothMesh.mVertices.size());
    physx::PxVec3 clothOffset = transform.getPosition();
    for(int i = 0; i < (int)clothMesh.mVertices.size(); i++)
    {
        // To put attachment point closer to each other
        if(clothMesh.mInvMasses[i] < 1e-6)
            clothMesh.mVertices[i] = (clothMesh.mVertices[i] - clothOffset)*0.9f + clothOffset;

        particlesCopy[i] = physx::PxVec4(clothMesh.mVertices[i], clothMesh.mInvMasses[i]); // w component is 1/mass, or 0.0f for anchored/fixed particles
    }
    
    // Create the cloth from the initial positions/masses and the fabric
    _clothActor[index].cloth = _factory->createCloth(nv::cloth::Range<physx::PxVec4>(&particlesCopy[0], &particlesCopy[0] + particlesCopy.size()), *_fabric[index]);
    
    // MARK: - Set distance constraints
    std::vector<physx::PxVec4> planes;
    planes.push_back(physx::PxVec4(physx::PxVec3(0.0f, 1.f, 0.0f), -0.01f));

    nv::cloth::Range<const physx::PxVec4> planesR(&planes[0], &planes[0] + planes.size());
    _clothActor[index].cloth->setPlanes(planesR, 0, _clothActor[index].cloth->getNumPlanes());
    std::vector<uint32_t> indices;
    indices.resize(planes.size());
    for(int i = 0; i < (int)indices.size(); i++)
        indices[i] = 1 << i;
    nv::cloth::Range<uint32_t> cind(&indices[0], &indices[0] + indices.size());
    _clothActor[index].cloth->setConvexes(cind, 0, _clothActor[index].cloth->getNumConvexes());

    _clothActor[index].cloth->setGravity(physx::PxVec3(0.0f, -1.0f, 0.0f));
    _clothActor[index].cloth->setFriction(0.1);
    _clothActor[index].cloth->setDragCoefficient(0.1);
    _clothActor[index].cloth->setLiftCoefficient(0.0);
    
    // Setup phase configs
    std::vector<nv::cloth::PhaseConfig> phases(_fabric[index]->getNumPhases());
    for (int i = 0; i < (int) phases.size(); i++) {
        phases[i].mPhaseIndex = i;
        phases[i].mStiffness = 0.6f;
        phases[i].mStiffnessMultiplier = 1.0f;
        phases[i].mCompressionLimit = 1.0f;
        phases[i].mStretchLimit = 1.0f;
    }
    _clothActor[index].cloth->setPhaseConfig(nv::cloth::Range<nv::cloth::PhaseConfig>(&phases.front(), &phases.back()));
    trackClothActor(&_clothActor[index]);
    
    // Add the cloth to the solver for simulation
    addClothToSolver(&_clothActor[index], _solver);
}

void FreeFallApp::loadScene() {
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
    renderer->setMesh(PrimitiveMesh::createSphere(_device.get(), 0.9));
    auto material = std::make_shared<BlinnPhongMaterial>();
    material->setBaseColor(Color(0.6, 0.4, 0.7, 1.0));
    renderer->setMaterial(material);
    
    auto clothEntity = rootEntity->createChild();
    float spaceX = -1.1f;
    for(int i = 0; i < 4; ++i) {
        _initializeCloth(clothEntity, i, physx::PxVec3(8.f + float((i+1)*(i+1)) * spaceX, 2.f, -7.f));
    }
}

}
}
