//
//  component_manager.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/6.
//

#ifndef component_manager_hpp
#define component_manager_hpp

#include "scene_forward.h"
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace vox {
namespace sg {
class ComponentManager {
public:
    std::unique_ptr<Component> get_model(uint32_t index = 0);
    
    void add_component(std::unique_ptr<Component> &&component);
        
    /**
     * @brief Set list of components for the given type
     * @param type_info The type of the component
     * @param components The list of components (retained)
     */
    void set_components(const std::type_index &type_info, std::vector<std::unique_ptr<Component>> &&components);
    
    /**
     * @brief Set list of components casted from the given template type
     */
    template<class T>
    void set_components(std::vector<std::unique_ptr<T>> &&components) {
        std::vector<std::unique_ptr<Component>> result(components.size());
        std::transform(components.begin(), components.end(), result.begin(),
                       [](std::unique_ptr<T> &component) -> std::unique_ptr<Component> {
            return std::unique_ptr<Component>(std::move(component));
        });
        set_components(typeid(T), std::move(result));
    }
    
    /**
     * @brief Clears a list of components
     */
    template<class T>
    void clear_components() {
        set_components(typeid(T), {});
    }
    
    /**
     * @return List of pointers to components casted to the given template type
     */
    template<class T>
    std::vector<T *> get_components() const {
        std::vector<T *> result;
        if (has_component(typeid(T))) {
            auto &scene_components = get_components(typeid(T));
            
            result.resize(scene_components.size());
            std::transform(scene_components.begin(), scene_components.end(), result.begin(),
                           [](const std::unique_ptr<Component> &component) -> T * {
                return dynamic_cast<T *>(component.get());
            });
        }
        
        return result;
    }
    
    /**
     * @return List of components for the given type
     */
    const std::vector<std::unique_ptr<Component>> &get_components(const std::type_index &type_info) const;
    
    template<class T>
    bool has_component() const {
        return has_component(typeid(T));
    }
    
    bool has_component(const std::type_index &type_info) const;
    
private:
    std::unordered_map<std::type_index, std::vector<std::unique_ptr<Component>>> components;
};

}        // namespace sg
}        // namespace vox

#endif /* component_manager_hpp */
