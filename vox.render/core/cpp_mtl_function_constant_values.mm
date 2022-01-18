//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cpp_mtl_function_constant_values.h"

using namespace MTL;

FunctionConstantValues::FunctionConstantValues() :
m_objCObj([MTLFunctionConstantValues new])  {
    // Member initialization only
}

FunctionConstantValues::FunctionConstantValues(const FunctionConstantValues &rhs) :
m_objCObj([rhs.m_objCObj copyWithZone:nil]) {
    // Member initialization only
}


FunctionConstantValues::~FunctionConstantValues() {
    m_objCObj = nil;
}

FunctionConstantValues &FunctionConstantValues::operator=(const FunctionConstantValues &rhs) {
    m_objCObj = [rhs.m_objCObj copyWithZone:nil];
    return *this;
}

void FunctionConstantValues::setConstantValue(const void *value, DataType type, UInteger index) {
    [m_objCObj setConstantValue:value type:(MTLDataType)type atIndex:index];
}

