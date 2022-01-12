//
//  shader_property.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/27.
//

#ifndef shader_property_hpp
#define shader_property_hpp

#include "shaderData_group.h"
#include <string>

namespace vox {
/**
 * Shader property.
 */
struct ShaderProperty {
    /** Shader property name. */
    const std::string name;
    
    const ShaderDataGroup group;
    
    const int uniqueId;
    
    ShaderProperty(const std::string &name, ShaderDataGroup group);
    
private:
    static int _propertyNameCounter;
};

}
#endif /* shader_property_hpp */
