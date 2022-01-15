//
//  scene_forward.h
//  DigitalArche
//
//  Created by 杨丰 on 2022/1/5.
//

#ifndef scene_forward_h
#define scene_forward_h

#include <memory>

namespace vox {
class Scene;

using ScenePtr = std::shared_ptr<Scene>;

class Entity;

using EntityPtr = std::shared_ptr<Entity>;

class Component;

class ComponentManager;

class Transform;

class SubMesh;

class Mesh;

using MeshPtr = std::shared_ptr<Mesh>;

class ModelMesh;

using ModelMeshPtr = std::shared_ptr<ModelMesh>;

class Material;

using MaterialPtr = std::shared_ptr<Material>;

class UnlitMaterial;

using UnlitMaterialPtr = std::shared_ptr<UnlitMaterial>;

class BlinnPhongMaterial;

using BlinnPhongMaterialPtr = std::shared_ptr<BlinnPhongMaterial>;

class PBRMaterial;

using PBRMaterialPtr = std::shared_ptr<PBRMaterial>;

class Camera;

class Renderer;

class MeshRenderer;

class GPUSkinnedMeshRenderer;

class Script;

class Animator;

class SceneAnimator;

class Light;

//class PointLight;

class SpotLight;

class DirectLight;

}

#endif /* scene_forward_h */
