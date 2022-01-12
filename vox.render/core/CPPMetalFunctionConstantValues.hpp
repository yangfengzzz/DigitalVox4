//
//  CPPMetalFunctionConstantValues.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/12.
//

#ifndef CPPMetalFunctionConstantValues_hpp
#define CPPMetalFunctionConstantValues_hpp

#include "CPPMetalTypes.hpp"
#include "CPPMetalImplementation.hpp"
#include "CPPMetalTypes.hpp"
#include "CPPMetalDataType.hpp"

namespace MTL {
class FunctionConstantValues {
public:
    FunctionConstantValues();
    
    FunctionConstantValues(const FunctionConstantValues &rhs);
    
    CPP_METAL_VIRTUAL ~FunctionConstantValues();
    
    FunctionConstantValues &operator=(const FunctionConstantValues &rhs);
    
    void setConstantValue(const void *value, DataType type, UInteger index);
    
private:
    CPPMetalInternal::FunctionConstantValues m_objCObj;
    
public: // Public methods for CPPMetal internal implementation
    CPPMetalInternal::FunctionConstantValues objCObj() const;
};

}

#endif /* CPPMetalFunctionConstantValues_hpp */
