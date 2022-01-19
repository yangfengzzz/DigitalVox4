//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef MTLLibrary_hpp
#define MTLLibrary_hpp

#include "cpp_mtl_implementation.h"
#include "cpp_mtl_types.h"
#include "cpp_mtl_device.h"
#include "cpp_mtl_function_constant_values.h"

namespace MTL {
class Function;

class Library {
public:
    Library();
    
    Library(const Library &rhs);
    
    Library(Library &&rhs);
    
    Library &operator=(const Library &rhs);
    
    Library &operator=(Library &&rhs);
    
    CPP_METAL_VIRTUAL ~Library();
    
    Function *newFunctionWithName(const char *name);
    
    Function makeFunction(const char *name);
    
    Function makeFunction(const char *name, const MTL::FunctionConstantValues& constantValues);
    
    const Device* device() const;
    
private:
    cpp_mtl_internal::Library m_objCObj;
    
    Device *m_device;
    
public: // Public methods for cpp_mtl_ internal implementation
    Library(cpp_mtl_internal::Library objCObj, Device &device);
    
};

class RenderPipelineState;

class RenderPipelineDescriptor;

typedef enum FunctionType {
    FunctionTypeVertex = 1,
    FunctionTypeFragment = 2,
    FunctionTypeKernel = 3,
    FunctionTypeVisible API_AVAILABLE(macos(11.0), ios(14.0)) = 5,
    FunctionTypeIntersection API_AVAILABLE(macos(11.0), ios(14.0)) = 6,
} FunctionType API_AVAILABLE(macos(10.11), ios(8.0));

class Function {
public:
    Function();
    
    Function(const Function &rhs);
    
    Function(Function &&rhs);
    
    Function &operator=(const Function &rhs);
    
    Function &operator=(Function &&rhs);
    
    CPP_METAL_VIRTUAL ~Function();
    
    const char *name() const;
    
    const Device* device() const;
    
private:
    
    cpp_mtl_internal::Function m_objCObj;
    
    Device *m_device;
    
public: // Public methods for cpp_mtl_ internal implementation
    
    Function(cpp_mtl_internal::Function objCObj, Device &device);
    
    cpp_mtl_internal::Function objCObj() const;
    
};


#pragma mark - Library inline method implementations

CPP_METAL_MOVE_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(Library);

#pragma mark - Function inline method implementations

CPP_METAL_MOVE_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(Function);

CPP_METAL_OBJCOBJ_GETTER_IMPLEMENATATION(Function);

} // namespace MTL

#endif // MTLLibrary_hpp
