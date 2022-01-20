//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cpp_mtl_library.h"
#include "cpp_mtl_device_internals.h"
#include "cpp_mtl_internal_macros.h"
#include <Metal/Metal.h>

using namespace MTL;

//==================================
#pragma mark - Metal Library Methods

CPP_METAL_CONSTRUCTOR_IMPLEMENTATION(Library);

CPP_METAL_NULL_REFERENCE_CONSTRUCTOR_IMPLEMENATATION(Library);

CPP_METAL_COPY_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(Library);

Library::~Library() {
    m_objCObj = nil;
}

Function *Library::newFunctionWithName(const char *name) {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    NSString *nsName = [[NSString alloc] initWithUTF8String:name];
    
    id <MTLFunction> objCObj = [m_objCObj newFunctionWithName:nsName];
    
    if (!objCObj) {
        NSLog(@"Error creating function '%s' from library", name);
        return nullptr;
    }
    
    Function *function = construct<Function>(m_device->allocator(), objCObj, *m_device);
    
    return function;
}

Function Library::makeFunction(const char *name) {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    NSString *nsName = [[NSString alloc] initWithUTF8String:name];
    
    id <MTLFunction> objCObj = [m_objCObj newFunctionWithName:nsName];
    
    if (!objCObj) {
        NSLog(@"Error creating function '%s' from library", name);
    }
    
    return Function(objCObj, *m_device);
}

Function Library::makeFunction(const char *name, const MTL::FunctionConstantValues& constantValues) {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    NSError *error = nil;
    NSString *nsName = [[NSString alloc] initWithUTF8String:name];
    
    id <MTLFunction> objCObj = [m_objCObj newFunctionWithName:nsName
                                               constantValues:constantValues.objCObj()
                                                        error:&error];
    if (error != nil) {
        NSLog(@"Error: failed to create Metal function: %@", error);
    }
    
    return Function(objCObj, *m_device);
}

Device* Library::device() {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    return m_device;
}

//===================================
#pragma mark - Metal Function Methods

CPP_METAL_CONSTRUCTOR_IMPLEMENTATION(Function);

CPP_METAL_NULL_REFERENCE_CONSTRUCTOR_IMPLEMENATATION(Function);

CPP_METAL_COPY_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(Function);

Function::~Function() {
    m_objCObj = nil;
}

CPP_METAL_DEVICE_GETTER_IMPLEMENTATION(Function);
