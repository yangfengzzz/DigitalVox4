//
//  CPPMetalFunctionConstantValues.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/12.
//

#include "CPPMetalFunctionConstantValues.hpp"

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

