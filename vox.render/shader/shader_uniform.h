//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.
#ifndef shader_uniform_hpp
#define shader_uniform_hpp

#include <string>
#include <vector>

namespace vox {
/**
 * Shader uniform。
 */
struct ShaderUniform {
    std::string name;
    int propertyId;
    size_t location;
    MTLFunctionType type;
};

}

#endif /* shader_uniform_hpp */
