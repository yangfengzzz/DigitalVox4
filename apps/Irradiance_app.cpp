//
//  Irradiance_app.cpp
//  apps
//
//  Created by 杨丰 on 2022/1/22.
//

#include "Irradiance_app.h"
#include "mesh/primitive_mesh.h"
#include "mesh/mesh_renderer.h"
#include "material/pbr_material.h"
#include "loader/texture_loader.h"
#include "camera.h"

namespace vox {
class BakerMaterial : public BaseMaterial {
public:
    BakerMaterial() : BaseMaterial(Shader::find("cubemapDebugger")) {
    }
    
    /// Base texture.
    MTL::TexturePtr baseTexture() {
        return std::any_cast<MTL::TexturePtr>(shaderData.getData(_baseTextureProp));
    }
    
    void setBaseTexture(MTL::TexturePtr newValue) {
        shaderData.setData(_baseTextureProp, newValue);
    }
    
    /// Tiling and offset of main textures.
    int faceIndex() {
        return std::any_cast<uint>(shaderData.getData(_faceIndexProp));
    }
    
    void setFaceInex(int newValue) {
        shaderData.setData(_faceIndexProp, newValue);
    }
    
private:
    ShaderProperty _baseTextureProp = Shader::createProperty("u_baseTexture", ShaderDataGroup::Material);
    ShaderProperty _faceIndexProp = Shader::createProperty("u_faceIndex", ShaderDataGroup::Material);
};

void IrradianceApp::loadScene(uint32_t width, uint32_t height) {
    auto rootEntity = _scene->createRootEntity();
    auto resourceLoader = TextureLoader(*_device);
    Shader::create("cubemapDebugger", "vertex_cubemap", "fragment_cubemap");

    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(0, 0, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    _mainCamera->resize(width, height);
    cameraEntity->addComponent<control::OrbitControl>();
    
    // Create Sphere
    auto sphereEntity = rootEntity->createChild("box");
    sphereEntity->transform->setPosition(-1, 2, 0);
    auto sphereMaterial = std::make_shared<PBRMaterial>();
    sphereMaterial->setRoughness(0);
    sphereMaterial->setMetallic(1);
    auto renderer = sphereEntity->addComponent<MeshRenderer>();
    renderer->setMesh(PrimitiveMesh::createSphere(_device.get(), 1, 64));
    renderer->setMaterial(sphereMaterial);
    
    // Create planes
    std::array<EntityPtr, 6> planes{};
    std::array<std::shared_ptr<BakerMaterial>, 6> planeMaterials{};
    
    for (int i = 0; i < 6; i++) {
        auto bakerEntity = rootEntity->createChild("IBL Baker Entity");
        bakerEntity->transform->setRotation(90, 0, 0);
        auto bakerMaterial = std::make_shared<BakerMaterial>();
        auto bakerRenderer = bakerEntity->addComponent<MeshRenderer>();
        bakerRenderer->setMesh(PrimitiveMesh::createPlane(_device.get(), 2, 2));
        bakerRenderer->setMaterial(bakerMaterial);
        planes[i] = bakerEntity;
        planeMaterials[i] = bakerMaterial;
    }
    
    planes[0]->transform->setPosition(1, 0, 0); // PX
    planes[1]->transform->setPosition(-3, 0, 0); // NX
    planes[2]->transform->setPosition(1, 2, 0); // PY
    planes[3]->transform->setPosition(1, -2, 0); // NY
    planes[4]->transform->setPosition(-1, 0, 0); // PZ
    planes[5]->transform->setPosition(3, 0, 0); // NZ

    const std::string path = "../assets/SkyMap/country";
    const std::array<std::string, 6> images = {"posx.png", "negx.png", "posy.png", "negy.png", "posz.png", "negz.png"};
    auto textures = resourceLoader.createSpecularTexture(path, images, true);
    _scene->ambientLight().setSpecularTexture(textures);
    
    auto changeMip = [&](MTL::UInteger mipLevel) {
        for (MTL::UInteger i = 0; i < 6; i++) {
            auto material = planeMaterials[i];
            auto planeTexture = textures->textureView(MTL::PixelFormatBGRA8Unorm_sRGB, MTL::TextureType2D,
                                                      MTL::Range{mipLevel, 1}, MTL::Range{i, 1});
            material->setBaseTexture(planeTexture);
            material->setFaceInex((int)i);
        }
    };
    changeMip(0);
}

}
