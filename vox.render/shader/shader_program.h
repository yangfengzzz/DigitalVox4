//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef shader_program_hpp
#define shader_program_hpp

#include "shader_macro_collection.h"
#include <Metal/Metal.hpp>

namespace vox {
/**
 * Shader program, corresponding to the GPU shader program.
 */
class ShaderProgram {
public:
    const std::shared_ptr<MTL::Function>& vertexShader() const;
    
    const std::shared_ptr<MTL::Function>& fragmentShader() const;
    
    /**
     * Whether this shader program is valid.
     */
    bool isValid() const;
    
    ShaderProgram(const std::shared_ptr<MTL::Library>& library,
                  const std::string &vertexSource, const std::string &fragmentSource,
                  const ShaderMacroCollection &macroInfo);
    
    
private:
    std::shared_ptr<MTL::FunctionConstantValues> makeFunctionConstants(const ShaderMacroCollection &macroInfo);
    
    /**
     * init and link program with shader.
     */
    void _createProgram(const std::string &vertexSource, const std::string &fragmentSource,
                        const ShaderMacroCollection &macroInfo);
    
    static int _counter;
    int ID;
    bool _isValid;
    const std::shared_ptr<MTL::Library>& _library;
    std::shared_ptr<MTL::Function> _vertexShader{nullptr};
    std::shared_ptr<MTL::Function> _fragmentShader{nullptr};
};

}

#endif /* shader_program_hpp */
