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
#include "rendering/compute_subpass.h"
#include "metal_helpers.h"

namespace vox {
namespace {
class AtomicMaterial: public BaseMaterial {
private:
    ShaderProperty _atomicProp;
    
public:
    AtomicMaterial():
    BaseMaterial(Shader::find("atomicRender")),
    _atomicProp(Shader::createProperty("u_atomic", ShaderDataGroup::Material)) {}
    
    void setAtomicBuffer(const std::shared_ptr<MTL::Buffer>& buffer) {
        shaderData.setData(_atomicProp, buffer);
    }
};

class AtomicComputeSubpass: public ComputeSubpass {
private:
    std::shared_ptr<MTL::Function> _atomicCompute{nullptr};
    std::shared_ptr<MTL::ComputePipelineState> _state{nullptr};
    std::shared_ptr<MTL::Buffer> _atomicBuffer{nullptr};
    
    AtomicMaterial* _material{nullptr};
    
public:
    AtomicComputeSubpass(RenderContext *context,
                         Scene *scene,
                         Camera *camera):
    ComputeSubpass(context, scene, camera) {
    }
    
    void setAtomicMaterial(AtomicMaterial* mat) {
        _material = mat;
    }
    
    void prepare() override {
        _atomicCompute =
        CLONE_METAL_CUSTOM_DELETER(MTL::Function,
                                   _pass->library().newFunction(NS::String::string("atomicCounter",
                                                                                   NS::StringEncoding::UTF8StringEncoding)));
        NS::Error* error;
        _state = CLONE_METAL_CUSTOM_DELETER(MTL::ComputePipelineState,
                                            _context->device().newComputePipelineState(_atomicCompute.get(), &error))
        
        _atomicBuffer = CLONE_METAL_CUSTOM_DELETER(MTL::Buffer, _context->device().newBuffer(sizeof(uint32_t), MTL::ResourceOptionCPUCacheModeDefault));
        _material->setAtomicBuffer(_atomicBuffer);
    }
    
    void compute(MTL::ComputeCommandEncoder &commandEncoder) override {
        commandEncoder.setComputePipelineState(_state.get());
        commandEncoder.setBuffer(_atomicBuffer.get(), 0, 0);
        commandEncoder.dispatchThreadgroups(MTL::Size(2,2,2), MTL::Size(1,1,1));
    }
};

} // namespace

bool AtomicComputeApp::prepare(Engine &engine) {
    ForwardApplication::prepare(engine);
    
    auto subpass = std::make_unique<AtomicComputeSubpass>(_renderContext.get(), _scene.get(), _mainCamera);
    subpass->setAtomicMaterial(static_cast<AtomicMaterial*>(_material.get()));
    _renderPass->addSubpass(std::move(subpass));
    
    return true;
}

void AtomicComputeApp::loadScene(uint32_t width, uint32_t height) {
    Shader::create("atomicRender", "vertex_unlit", "fragment_atomic");
    
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
    
    auto cubeEntity = rootEntity->createChild();
    auto renderer = cubeEntity->addComponent<MeshRenderer>();
    renderer->setMesh(PrimitiveMesh::createCuboid(*_device, 1));
    _material = std::make_shared<AtomicMaterial>();
    renderer->setMaterial(_material);
}

}
