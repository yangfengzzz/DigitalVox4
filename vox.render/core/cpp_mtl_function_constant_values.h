//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cpp_mtl_FunctionConstantValues_hpp
#define cpp_mtl_FunctionConstantValues_hpp

#include "cpp_mtl_types.h"
#include "cpp_mtl_implementation.h"
#include "cpp_mtl_types.h"
#include "cpp_mtl_data_type.h"

namespace MTL {
class FunctionConstantValues {
public:
    FunctionConstantValues();
    
    FunctionConstantValues(const FunctionConstantValues &rhs);
    
    CPP_METAL_VIRTUAL ~FunctionConstantValues();
    
    FunctionConstantValues &operator=(const FunctionConstantValues &rhs);
    
    void setConstantValue(const void *value, DataType type, UInteger index);
    
private:
    cpp_mtl_internal::FunctionConstantValues m_objCObj;
    
public: // Public methods for cpp_mtl_ internal implementation
    cpp_mtl_internal::FunctionConstantValues objCObj() const;
};

//===============================================
#pragma mark - View inline method implementations

inline cpp_mtl_internal::FunctionConstantValues FunctionConstantValues::objCObj() const {
    return m_objCObj;
}

}

#endif /* cpp_mtl_FunctionConstantValues_hpp */
