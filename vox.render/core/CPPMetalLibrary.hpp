/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for C++ Metal library and function class wrappers
*/

#ifndef MTLLibrary_hpp
#define MTLLibrary_hpp

#include "CPPMetalImplementation.hpp"
#include "CPPMetalTypes.hpp"
#include "CPPMetalDevice.hpp"


namespace MTL
{


class Function;

class Library
{
public:

    Library();

    Library(const Library & rhs);

    Library(Library && rhs);

    Library & operator=(const Library & rhs);

    Library & operator=(Library && rhs);

    CPP_METAL_VIRTUAL ~Library();

    Function *newFunctionWithName(const char* name);
    Function makeFunction(const char* name);

    Device device() const;

private:

    CPPMetalInternal::Library m_objCObj;

    Device *m_device;

public: // Public methods for CPPMetal internal implementation

    Library(CPPMetalInternal::Library objCObj, Device & device);

};

class RenderPipelineState;
class RenderPipelineDescriptor;

class Function
{
public:

    Function();

    Function(const Function & rhs);

    Function(Function && rhs);

    Function & operator=(const Function & rhs);

    Function & operator=(Function && rhs);

    CPP_METAL_VIRTUAL ~Function();

    const char* name() const;

    Device device() const;

private:

    CPPMetalInternal::Function m_objCObj;

    Device *m_device;

public: // Public methods for CPPMetal internal implementation

    Function(CPPMetalInternal::Function objCObj, Device & device);

    CPPMetalInternal::Function objCObj() const;

};


//==================================================
#pragma mark - Library inline method implementations

CPP_METAL_MOVE_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(Library);

//===================================================
#pragma mark - Function inline method implementations

CPP_METAL_MOVE_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(Function);

CPP_METAL_OBJCOBJ_GETTER_IMPLEMENATATION(Function);

} // namespace MTL

#endif // MTLLibrary_hpp
