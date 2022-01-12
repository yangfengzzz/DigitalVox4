//
//  component_manager.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/6.
//

#include "component_manager.h"
#include "component.h"
//#include "components/mesh.h"

namespace vox {
namespace sg {
std::unique_ptr<Component> ComponentManager::get_model(uint32_t index) {
//    auto meshes = std::move(components.at(typeid(Mesh)));
    
//    return std::move(meshes.at(index));
    
    return nullptr;
}

void ComponentManager::add_component(std::unique_ptr<Component> &&component) {
    if (component) {
        components[component->get_type()].push_back(std::move(component));
    }
}

void ComponentManager::set_components(const std::type_index &type_info,
                                      std::vector<std::unique_ptr<Component>> &&new_components) {
    components[type_info] = std::move(new_components);
}

const std::vector<std::unique_ptr<Component>> &ComponentManager::get_components(const std::type_index &type_info) const {
    return components.at(type_info);
}

bool ComponentManager::has_component(const std::type_index &type_info) const {
    auto component = components.find(type_info);
    return (component != components.end() && !component->second.empty());
}

}        // namespace sg
}        // namespace vox
