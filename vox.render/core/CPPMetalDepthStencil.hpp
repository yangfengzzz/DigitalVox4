/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for C++ Metal depth stencil state class wrappers
*/

#ifndef CPPMetalDepthStencilState_hpp
#define CPPMetalDepthStencilState_hpp

#include <CoreFoundation/CoreFoundation.h>
#include "CPPMetalImplementation.hpp"
#include "CPPMetalPixelFormat.hpp"
#include "CPPMetalConstants.hpp"
#include "CPPMetalTypes.hpp"
#include "CPPMetalDevice.hpp"


namespace MTL
{


typedef enum CompareFunction {
    CompareFunctionNever = 0,
    CompareFunctionLess = 1,
    CompareFunctionEqual = 2,
    CompareFunctionLessEqual = 3,
    CompareFunctionGreater = 4,
    CompareFunctionNotEqual = 5,
    CompareFunctionGreaterEqual = 6,
    CompareFunctionAlways = 7,
} CompareFunction API_AVAILABLE(macos(10.11), ios(8.0));

typedef enum StencilOperation {
    StencilOperationKeep = 0,
    StencilOperationZero = 1,
    StencilOperationReplace = 2,
    StencilOperationIncrementClamp = 3,
    StencilOperationDecrementClamp = 4,
    StencilOperationInvert = 5,
    StencilOperationIncrementWrap = 6,
    StencilOperationDecrementWrap = 7,
} StencilOperation API_AVAILABLE(macos(10.11), ios(8.0));

class Device;
class DepthStencilDescriptor;

struct StencilDescriptor
{
public:

    StencilDescriptor();

    StencilDescriptor(const StencilDescriptor & rhs);

    StencilDescriptor & operator=(const StencilDescriptor & rhs);

    CPP_METAL_VIRTUAL ~StencilDescriptor();

    CompareFunction stencilCompareFunction() const;
    void            stencilCompareFunction(CompareFunction function);

    StencilOperation stencilFailureOperation() const;
    void             stencilFailureOperation(StencilOperation operation);

    StencilOperation depthFailureOperation() const;
    void             depthFailureOperation(StencilOperation operation);

    StencilOperation depthStencilPassOperation() const;
    void             depthStencilPassOperation(StencilOperation operation);

    uint32_t readMask() const;
    void     readMask(uint32_t mask);

    uint32_t writeMask() const;
    void     writeMask(uint32_t mask);

private:

    CPPMetalInternal::StencilDescriptor m_objCObj;

    CPPMetalInternal::DepthStencilDescriptor m_objCContainer;

    Device *m_device;

    bool m_frontFace;

    StencilDescriptor(CPPMetalInternal::DepthStencilDescriptor objCContainer, bool frontFace);

    void reinitialize(CPPMetalInternal::DepthStencilDescriptor objCContainer);

    friend DepthStencilDescriptor;

public: // Public methods for CPPMetal internal implementation

    CPPMetalInternal::StencilDescriptor objCObj() const;

};

class DepthStencilDescriptor
{
private:

    // CPPMetal Internal Implementation Note:
    // The class must declare the 'm_objCObj' member before the 'frontFaceStencil' and
    // 'backFaceStencil" members.  The constructor of these members depend on the value of
    // properties in 'm_objCObj'.  C++ executes member constructors in order of declaration (not in
    // order of the initializer list in the contstructor).  So the class must declare 'm_objCObj'
    // first for it to initialize it first and allow these members to depend on its properties.
    CPPMetalInternal::DepthStencilDescriptor m_objCObj;

public:

    DepthStencilDescriptor();

    DepthStencilDescriptor(const DepthStencilDescriptor & rhs);

    DepthStencilDescriptor & operator=(const DepthStencilDescriptor & rhs);

    CPP_METAL_VIRTUAL ~DepthStencilDescriptor();

    CompareFunction depthCompareFunction() const;
    void            depthCompareFunction(CompareFunction function);

    bool isDepthWriteEnabled() const;
    void depthWriteEnabled(bool enabled);

    const char* label() const;
    void        label(const CFStringRef string);
    void        label(const char* string);

    StencilDescriptor frontFaceStencil;

    StencilDescriptor backFaceStencil;

public: // Public methods for CPPMetal internal implementation

    CPPMetalInternal::DepthStencilDescriptor objCObj() const;

};

class DepthStencilState
{
public:

    DepthStencilState();

    DepthStencilState(const DepthStencilState & rhs);

    DepthStencilState(DepthStencilState && rhs);

    DepthStencilState & operator=(const DepthStencilState & rhs);

    DepthStencilState & operator=(DepthStencilState && rhs);

    CPP_METAL_VIRTUAL ~DepthStencilState();

    bool operator==(const DepthStencilState & rhs) const;

    const char* label() const;

    Device device() const;

private:

    CPPMetalInternal::DepthStencilState m_objCObj;

    Device *m_device;

public: // Public methods for CPPMetal internal implementation

    DepthStencilState(CPPMetalInternal::DepthStencilState objCObj, Device & device);

    CPPMetalInternal::DepthStencilState objCObj() const;

};


//============================================================
#pragma mark - StencilDescriptor inline method implementations

CPP_METAL_OBJCOBJ_GETTER_IMPLEMENATATION(StencilDescriptor);


//=================================================================
#pragma mark - DepthStencilDescriptor inline method implementations

inline void DepthStencilDescriptor::label(const char* string)
{
    CPP_METAL_PROCESS_LABEL(string, label);
}

CPP_METAL_OBJCOBJ_GETTER_IMPLEMENATATION(DepthStencilDescriptor);


//============================================================
#pragma mark - DepthStencilState inline method implementations

CPP_METAL_MOVE_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(DepthStencilState);

CPP_METAL_OBJCOBJ_GETTER_IMPLEMENATATION(DepthStencilState);

} // namespace MTL

#endif // CPPMetalDepthStencilState_hpp
