//
//  shader_program.cpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/27.
//

#include "shader_program.h"
#include <string>

namespace vox {
int ShaderProgram::_counter = 0;

MTL::Function ShaderProgram::vertexShader() {
    return _vertexShader;
}

MTL::Function ShaderProgram::fragmentShader() {
    return _fragmentShader;
}

bool ShaderProgram::isValid() {
    return _isValid;
}

ShaderProgram::ShaderProgram(MTL::Library* library,
                             const std::string &vertexSource,
                             const std::string &fragmentSource,
                             const ShaderMacroCollection &macroInfo) {
    ID = ShaderProgram::_counter;
    ShaderProgram::_counter += 1;
    
    _library = library;
    _createProgram(vertexSource, fragmentSource, macroInfo);
    _isValid = true;
}

MTL::FunctionConstantValues ShaderProgram::makeFunctionConstants(const ShaderMacroCollection &macroInfo) {
    auto functionConstants = ShaderMacroCollection::createDefaultFunction();
    std::for_each(macroInfo._value.begin(), macroInfo._value.end(),
                  [&](const std::pair<MacroName, std::pair<int, MTL::DataType>> &info) {
        if (info.second.second == MTL::DataTypeBool) {
            bool property;
            if (info.second.first == 1) {
                property = true;
            } else {
                property = false;
            }
            functionConstants.setConstantValue(&property, MTL::DataTypeBool, info.first);
        } else {
            auto &property = info.second.first;
            functionConstants.setConstantValue(&property, info.second.second, info.first);
        }
    });
    return functionConstants;
}

void ShaderProgram::_createProgram(const std::string &vertexSource, const std::string &fragmentSource,
                                   const ShaderMacroCollection &macroInfo) {
    auto functionConstants = makeFunctionConstants(macroInfo);
    _vertexShader = _library->makeFunction(vertexSource.c_str(), functionConstants);
    _fragmentShader = _library->makeFunction(fragmentSource.c_str(), functionConstants);
}


}
