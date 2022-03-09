//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "atomic_compute.h"
#include "entity.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/unlit_material.h"
#include "material/blinn_phong_material.h"
#include "camera.h"
#include "controls/orbit_control.h"
#include "image/stb.h"
#include "metal_helpers.h"

namespace vox {
namespace {
class AtomicMaterial: public BaseMaterial {
private:
    ShaderProperty _atomicProp;
    std::shared_ptr<MTL::Buffer> _atomicBuffer;
    
public:
    AtomicMaterial(MTL::Device& device):
    BaseMaterial(Shader::find("atomicRender")),
    _atomicProp(Shader::createProperty("u_atomic", ShaderDataGroup::Material)) {
        _atomicBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, device.newBuffer(sizeof(uint32_t),
                                                                                 MTL::ResourceOptionCPUCacheModeDefault));
        setAtomicBuffer(_atomicBuffer);
    }
    
    const std::shared_ptr<MTL::Buffer>& atomicBuffer() {
        return _atomicBuffer;
    }
    
    void setAtomicBuffer(const std::shared_ptr<MTL::Buffer>& buffer) {
        shaderData.setBufferFunctor(_atomicProp, [this]()->auto {
            return _atomicBuffer;
        });
    }
};

} // namespace

bool AtomicComputeApp::prepare(Engine &engine) {
    ForwardApplication::prepare(engine);
    
    _pass = std::make_unique<ComputePass>(*_library, _scene.get(), "atomicCounter");
    _pass->setThreadsPerGrid(2, 2, 2);
    _pass->attachShaderData(&_material->shaderData);
    
    return true;
}

void AtomicComputeApp::loadScene() {
    Shader::create("atomicRender", "vertex_unlit", "fragment_atomic");
    
    auto rootEntity = _scene->createRootEntity();
    
    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();
    
    // init point light
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(0, 3, 0);
    auto pointLight = light->addComponent<PointLight>();
    pointLight->intensity = 0.3;
    
    auto cubeEntity = rootEntity->createChild();
    auto renderer = cubeEntity->addComponent<MeshRenderer>();
    renderer->setMesh(PrimitiveMesh::createCuboid(*_device, 1));
    _material = std::make_shared<AtomicMaterial>(*_device);
    renderer->setMaterial(_material);
}

void AtomicComputeApp::updateGPUTask(MTL::CommandBuffer& commandBuffer) {
    ForwardApplication::updateGPUTask(commandBuffer);
    
    auto encoder = CLONE_METAL_CUSTOM_DELETER(MTL::ComputeCommandEncoder, commandBuffer.computeCommandEncoder());
    _pass->compute(*encoder);
    encoder->endEncoding();
}

}
