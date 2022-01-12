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
namespace sg {
class Scene;

class Entity;

using EntityPtr = std::shared_ptr<Entity>;

class Component;

class ComponentManager;

class Script;

class Camera;

class Transform;

class Light;

class Renderer;

class Mesh;

class SubMesh;

class Material;

class PBRMaterial;

class Texture;

class Image;

class Sampler;

}
}

#endif /* scene_forward_h */
