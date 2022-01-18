//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cpp_mtl_CommandEncoeder_happ
#define cpp_mtl_CommandEncoeder_happ

#include "cpp_mtl_implementation.h"
#include "cpp_mtl_pixel_format.h"
#include "cpp_mtl_types.h"
#include "cpp_mtl_device.h"


namespace MTL {
class Device;

class CommandEncoder {
public:
    CommandEncoder() = delete;
    
    CommandEncoder(const CommandEncoder &rhs);
    
    CommandEncoder(CommandEncoder &&rhs);
    
    CommandEncoder &operator=(const CommandEncoder &rhs);
    
    CommandEncoder &operator=(CommandEncoder &&rhs);
    
    virtual ~CommandEncoder();
    
    void endEncoding();
    
    const char *label() const;
    
    void label(const char *string);
    
    void label(const CFStringRef string);
    
    void pushDebugGroup(const char *string);
    
    void pushDebugGroup(const CFStringRef string);
    
    void popDebugGroup();
    
    const Device* device() const;
    
protected:
    CommandEncoder(cpp_mtl_Internal::CommandEncoder objCObj, Device &device);
    
    Device *m_device;
    
    cpp_mtl_Internal::CommandEncoder m_objCObj;
    
};

#pragma mark - CommandEncoder inline method implementations

CPP_METAL_MOVE_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(CommandEncoder);

inline void CommandEncoder::pushDebugGroup(const char *string) {
    CPP_METAL_PROCESS_LABEL(string, pushDebugGroup);
}

inline void CommandEncoder::label(const char *string) {
    CPP_METAL_PROCESS_LABEL(string, label);
}


} // namespace MTL

#endif // cpp_mtl_CommandEncoeder_hpp
