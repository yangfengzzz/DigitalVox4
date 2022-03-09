//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "distance_constraint_app.h"
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

void DistanceConstraintApp::_initializeCloth(EntityPtr entity, physx::PxVec3 offset) {
    _solver = _factory->createSolver();
    trackSolver(_solver);
    
    ClothMeshData clothMesh;
    physx::PxMat44 transform = PxTransform(PxVec3(0.f, 10.f, 0.f) + offset, PxQuat(0, PxVec3(1.f, 0.f, 0.f)));
    clothMesh.GeneratePlaneCloth(5.f, 6.f, 20, 20, false, transform);
    //    clothMesh.AttachClothPlaneByAngles(69, 79);
    
    nv::cloth::ClothMeshDesc meshDesc = clothMesh.GetClothMeshDesc();
    nv::cloth::Vector<int32_t>::Type phaseTypeInfo;
    _fabric = NvClothCookFabricFromMesh(_factory, meshDesc, physx::PxVec3(0.0f, -1.0f, 0.0f), &phaseTypeInfo, false);
    trackFabric(_fabric);
    
    _clothActor.clothRenderer = entity->addComponent<ClothRenderer>();
    _clothActor.clothRenderer->setClothMeshDesc(meshDesc);
    auto material = std::make_shared<BlinnPhongMaterial>();
    material->setRenderFace(RenderFace::Double);
    material->setBaseColor(Color(247/256.0, 186/256.0, 11/256.0));
    _clothActor.clothRenderer->setMaterial(material);
    
    // Initialize start positions and masses for the actual cloth instance
    // (note: the particle/vertex positions do not have to match the mesh description here. Set the positions to the initial shape of this cloth instance)
    std::vector<physx::PxVec4> particlesCopy;
    particlesCopy.resize(clothMesh.mVertices.size());
    for (int i = 0; i < (int) clothMesh.mVertices.size(); i++) {
        particlesCopy[i] = physx::PxVec4(clothMesh.mVertices[i], clothMesh.mInvMasses[i]); // w component is 1/mass, or 0.0f for anchored/fixed particles
    }
    // Create the cloth from the initial positions/masses and the fabric
    _clothActor.cloth = _factory->createCloth(nv::cloth::Range<physx::PxVec4>(&particlesCopy[0], &particlesCopy[0] + particlesCopy.size()), *_fabric);
    
    // MARK: - Set distance constraints
    nv::cloth::Range<physx::PxVec4> distanceConstraints = _clothActor.cloth->getMotionConstraints();
    for(int i = 0; i < (int)distanceConstraints.size(); i++) {
        distanceConstraints[i] = physx::PxVec4(particlesCopy[i].getXYZ(), 0.002f*(i % 800) * 0.002f*(i % 800));
    }
    _clothActor.cloth->setGravity(physx::PxVec3(0.0f, -9.8f, 0.0f));
    
    // Setup phase configs
    std::vector<nv::cloth::PhaseConfig> phases(_fabric->getNumPhases());
    for (int i = 0; i < (int) phases.size(); i++) {
        phases[i].mPhaseIndex = i;
        phases[i].mStiffness = 0.6f;
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

void DistanceConstraintApp::loadScene() {
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
    _initializeCloth(clothEntity, physx::PxVec3(0.0f, 0.0f, 0.0f));
}

}
}
