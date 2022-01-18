//
//  shader_macro_collection.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/27.
//

#ifndef shader_macro_collection_hpp
#define shader_macro_collection_hpp

#include <unordered_map>
#include "macro_name.h"
#include "core/cpp_mtl_function_constant_values.h"

namespace vox {
/**
 * Shader macro collection.
 */
struct ShaderMacroCollection {
    static std::unordered_map<MacroName, std::pair<int, MTL::DataType>> defaultValue;
    
    static MTL::FunctionConstantValues createDefaultFunction();
    
    /**
     * Union of two macro collection.
     * @param left - input macro collection
     * @param right - input macro collection
     * @param result - union output macro collection
     */
    static void unionCollection(const ShaderMacroCollection &left, const ShaderMacroCollection &right,
                                ShaderMacroCollection &result);
    
    size_t hash() const;
    
private:
    friend class ShaderProgram;
    
    friend class ShaderData;
    
    std::unordered_map<MacroName, std::pair<int, MTL::DataType>> _value{};
};

}

#endif /* shader_macro_collection_hpp */
