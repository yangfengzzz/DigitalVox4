//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cpp_mtl_DepthStencilState_hpp
#define cpp_mtl_DepthStencilState_hpp

#include <CoreFoundation/CoreFoundation.h>
#include "cpp_mtl_implementation.h"
#include "cpp_mtl_pixel_format.h"
#include "cpp_mtl_constants.h"
#include "cpp_mtl_types.h"
#include "cpp_mtl_device.h"


namespace MTL {
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

struct StencilDescriptor {
public:
    StencilDescriptor();
    
    StencilDescriptor(const StencilDescriptor &rhs);
    
    StencilDescriptor &operator=(const StencilDescriptor &rhs);
    
    CPP_METAL_VIRTUAL ~StencilDescriptor();
    
    CompareFunction stencilCompareFunction() const;
    
    void stencilCompareFunction(CompareFunction function);
    
    StencilOperation stencilFailureOperation() const;
    
    void stencilFailureOperation(StencilOperation operation);
    
    StencilOperation depthFailureOperation() const;
    
    void depthFailureOperation(StencilOperation operation);
    
    StencilOperation depthStencilPassOperation() const;
    
    void depthStencilPassOperation(StencilOperation operation);
    
    uint32_t readMask() const;
    
    void readMask(uint32_t mask);
    
    uint32_t writeMask() const;
    
    void writeMask(uint32_t mask);
    
private:
    cpp_mtl_internal::StencilDescriptor m_objCObj;
    
    cpp_mtl_internal::DepthStencilDescriptor m_objCContainer;
    
    Device *m_device;
    
    bool m_frontFace;
    
    StencilDescriptor(cpp_mtl_internal::DepthStencilDescriptor objCContainer, bool frontFace);
    
    void reinitialize(cpp_mtl_internal::DepthStencilDescriptor objCContainer);
    
    friend DepthStencilDescriptor;
    
public: // Public methods for cpp_mtl_ internal implementation
    cpp_mtl_internal::StencilDescriptor objCObj() const;
    
};

class DepthStencilDescriptor {
private:
    // cpp_mtl_ Internal Implementation Note:
    // The class must declare the 'm_objCObj' member before the 'frontFaceStencil' and
    // 'backFaceStencil" members.  The constructor of these members depend on the value of
    // properties in 'm_objCObj'.  C++ executes member constructors in order of declaration (not in
    // order of the initializer list in the contstructor).  So the class must declare 'm_objCObj'
    // first for it to initialize it first and allow these members to depend on its properties.
    cpp_mtl_internal::DepthStencilDescriptor m_objCObj;
    
public:
    DepthStencilDescriptor();
    
    DepthStencilDescriptor(const DepthStencilDescriptor &rhs);
    
    DepthStencilDescriptor &operator=(const DepthStencilDescriptor &rhs);
    
    CPP_METAL_VIRTUAL ~DepthStencilDescriptor();
    
    CompareFunction depthCompareFunction() const;
    
    void depthCompareFunction(CompareFunction function);
    
    bool isDepthWriteEnabled() const;
    
    void depthWriteEnabled(bool enabled);
    
    const char *label() const;
    
    void label(const CFStringRef string);
    
    void label(const char *string);
    
    StencilDescriptor frontFaceStencil;
    
    StencilDescriptor backFaceStencil;
    
public: // Public methods for cpp_mtl_ internal implementation
    
    cpp_mtl_internal::DepthStencilDescriptor objCObj() const;
    
};

class DepthStencilState {
public:
    DepthStencilState();
    
    DepthStencilState(const DepthStencilState &rhs);
    
    DepthStencilState(DepthStencilState &&rhs);
    
    DepthStencilState &operator=(const DepthStencilState &rhs);
    
    DepthStencilState &operator=(DepthStencilState &&rhs);
    
    CPP_METAL_VIRTUAL ~DepthStencilState();
    
    bool operator==(const DepthStencilState &rhs) const;
    
    const char *label() const;
    
    const Device* device() const;
    
private:
    cpp_mtl_internal::DepthStencilState m_objCObj;
    
    Device *m_device;
    
public: // Public methods for cpp_mtl_ internal implementation
    DepthStencilState(cpp_mtl_internal::DepthStencilState objCObj, Device &device);
    
    cpp_mtl_internal::DepthStencilState objCObj() const;
    
};


#pragma mark - StencilDescriptor inline method implementations
CPP_METAL_OBJCOBJ_GETTER_IMPLEMENATATION(StencilDescriptor);


#pragma mark - DepthStencilDescriptor inline method implementations
inline void DepthStencilDescriptor::label(const char *string) {
    CPP_METAL_PROCESS_LABEL(string, label);
}

CPP_METAL_OBJCOBJ_GETTER_IMPLEMENATATION(DepthStencilDescriptor);


#pragma mark - DepthStencilState inline method implementations
CPP_METAL_MOVE_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(DepthStencilState);

CPP_METAL_OBJCOBJ_GETTER_IMPLEMENATATION(DepthStencilState);

} // namespace MTL

#endif // cpp_mtl_DepthStencilState_hpp
