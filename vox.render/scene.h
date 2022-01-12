/* Copyright (c) 2018-2020, Arm Limited and Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 the "License";
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "scene_forward.h"
#include "components_manager.h"
#include "shader/shader_data.h"
//#include "scene_graph/components/light.h"
//#include "scene_graph/components/texture.h"

namespace vox {
namespace sg {
/// @brief A collection of entities organized in a tree structure.
///		   It can contain more than one root entity.
class Scene {
public:
    /** Scene name. */
    std::string name;
    
    ComponentsManager _componentsManager;
    
    /** The background of the scene. */
//    Background background = Background(_engine);
    
    /** Scene-related shader data. */
    ShaderData shaderData = ShaderData();
    
    /** Light Manager */
//    LightManager light_manager;
    
    /**
     * Create scene.
     * @param name - Name
     */
    Scene(std::string name = "");
    
    /**
     * Ambient light.
     */
//    AmbientLight &ambientLight();
    
    /**
     * Count of root entities.
     */
    size_t rootEntitiesCount();
    
    /**
     * Root entity collection.
     */
    const std::vector<EntityPtr> &rootEntities();
    
    /**
     * Whether it's destroyed.
     */
    bool destroyed();
    
    /**
     * Create root entity.
     * @param name - Entity name
     * @returns Entity
     */
    EntityPtr createRootEntity(std::string name = "");
    
    /**
     * Append an entity.
     * @param entity - The root entity to add
     */
    void addRootEntity(EntityPtr entity);
    
    /**
     * Remove an entity.
     * @param entity - The root entity to remove
     */
    void removeRootEntity(EntityPtr entity);
    
    /**
     * Get root entity from index.
     * @param index - Index
     * @returns Entity
     */
    EntityPtr getRootEntity(size_t index = 0);
    
    /**
     * Find entity globally by name.
     * @param name - Entity name
     * @returns Entity
     */
    EntityPtr findEntityByName(const std::string &name);
    
    /**
     * Destroy this scene.
     */
    void destroy();
    
    void attachRenderCamera(Camera *camera);
    
    void detachRenderCamera(Camera *camera);
    
private:
    void _processActive(bool active);
    
    void _updateShaderData();
    
    void _removeEntity(EntityPtr entity);
    
    std::vector<Camera *> _activeCameras;
    ShaderMacroCollection _globalShaderMacro = ShaderMacroCollection();
    
    bool _destroyed = false;
    std::vector<EntityPtr> _rootEntities;
//    AmbientLight _ambientLight;
};
}        // namespace sg
}        // namespace vox
