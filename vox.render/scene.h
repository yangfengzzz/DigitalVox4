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
//#include "scene_graph/components/light.h"
//#include "scene_graph/components/texture.h"

namespace vox {
namespace sg {
/// @brief A collection of entities organized in a tree structure.
///		   It can contain more than one root entity.
class Scene {
public:
    Scene() = default;
    
    Scene(const std::string &name);
    
    void set_name(const std::string &name);
    
    const std::string &get_name() const;
    
    void set_entities(std::vector<std::unique_ptr<Entity>> &&entities);
    
    void add_entity(std::unique_ptr<Entity> &&entity);
    
    void add_child(Entity &child);
    
    Entity *find_entity(const std::string &name);
    
    void set_root_entity(Entity &entity);
    
    Entity &get_root_entity();
    
private:
    std::string name;
    
    /// List of all the entities
    std::vector<std::unique_ptr<Entity>> entities;
    
    Entity *root{nullptr};    
};
}        // namespace sg
}        // namespace vox
