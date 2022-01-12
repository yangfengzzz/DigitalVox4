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

#include "scene.h"

#include <queue>

//#include "common/error.h"
#include "entity.h"

namespace vox {
namespace sg {
Scene::Scene(const std::string &name) :
name{name} {
}

void Scene::set_name(const std::string &new_name) {
    name = new_name;
}

const std::string &Scene::get_name() const {
    return name;
}

void Scene::set_entities(std::vector<std::unique_ptr<Entity>> &&n) {
    assert(entities.empty() && "Scene nodes were already set");
    entities = std::move(n);
}

void Scene::add_entity(std::unique_ptr<Entity> &&n) {
    entities.emplace_back(std::move(n));
}

void Scene::add_child(Entity &child) {
    root->add_child(child);
}

Entity *Scene::find_entity(const std::string &entity_name) {
    for (auto root_node: root->get_children()) {
        std::queue<sg::Entity *> traverse_nodes{};
        traverse_nodes.push(root_node);
        
        while (!traverse_nodes.empty()) {
            auto node = traverse_nodes.front();
            traverse_nodes.pop();
            
            if (node->get_name() == entity_name) {
                return node;
            }
            
            for (auto child_node: node->get_children()) {
                traverse_nodes.push(child_node);
            }
        }
    }
    
    return nullptr;
}

void Scene::set_root_entity(Entity &entity) {
    root = &entity;
}

Entity &Scene::get_root_entity() {
    return *root;
}
}        // namespace sg
}        // namespace vox
