//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shader_program.h"
#include "metal_helpers.h"
#include <string>

namespace vox {
int ShaderProgram::_counter = 0;

const std::shared_ptr<MTL::Function> &ShaderProgram::vertexShader() const {
    return _vertexShader;
}

const std::shared_ptr<MTL::Function> &ShaderProgram::fragmentShader() const {
    return _fragmentShader;
}

bool ShaderProgram::isValid() const {
    return _isValid;
}

ShaderProgram::ShaderProgram(MTL::Library &library,
                             const std::string &vertexSource,
                             const std::string &fragmentSource,
                             const ShaderMacroCollection &macroInfo) :
_library(library) {
    ID = ShaderProgram::_counter;
    ShaderProgram::_counter += 1;
    
    _createProgram(vertexSource, fragmentSource, macroInfo);
    _isValid = true;
}

std::shared_ptr<MTL::FunctionConstantValues> ShaderProgram::makeFunctionConstants(const ShaderMacroCollection &macroInfo) {
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
            functionConstants->setConstantValue(&property, MTL::DataTypeBool, info.first);
        } else {
            auto &property = info.second.first;
            functionConstants->setConstantValue(&property, info.second.second, info.first);
        }
    });
    return functionConstants;
}

void ShaderProgram::_createProgram(const std::string &vertexSource, const std::string &fragmentSource,
                                   const ShaderMacroCollection &macroInfo) {
    NS::Error *error;
    auto functionConstants = makeFunctionConstants(macroInfo);
    if (vertexSource != "") {
        auto shader = _library.newFunction(NS::String::string(vertexSource.c_str(),
                                                              NS::StringEncoding::UTF8StringEncoding),
                                           functionConstants.get(), &error);
        _vertexShader = CLONE_METAL_CUSTOM_DELETER(MTL::Function, shader);
    }
    if (fragmentSource != "") {
        auto shader = _library.newFunction(NS::String::string(fragmentSource.c_str(),
                                                              NS::StringEncoding::UTF8StringEncoding),
                                           functionConstants.get(), &error);
        _fragmentShader = CLONE_METAL_CUSTOM_DELETER(MTL::Function, shader);
    }
}


}
