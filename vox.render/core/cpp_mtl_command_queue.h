//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cpp_mtl_CommandQueue_hpp
#define cpp_mtl_CommandQueue_hpp

#include "cpp_mtl_implementation.h"
#include "cpp_mtl_types.h"
#include "cpp_mtl_constants.h"


namespace MTL {
class Device;

class CommandBuffer;

class CommandQueue {
public:
    CommandQueue();
    
    CommandQueue(const CommandQueue &rhs);
    
    CommandQueue(CommandQueue &&rhs);
    
    CommandQueue &operator=(const CommandQueue &rhs);
    
    CommandQueue &operator=(CommandQueue &&rhs);
    
    CPP_METAL_VIRTUAL ~CommandQueue();
    
    void endEncoding();
    
    const char *label() const;
    
    void label(const char *string);
    
    void label(const CFStringRef string);
    
    const Device* device() const;
    
    CommandBuffer commandBuffer();
    
private:
    cpp_mtl_Internal::CommandQueue m_objCObj;
    
    Device *m_device;
    
public: // Public methods for cpp_mtl_ internal implementation
    CommandQueue(cpp_mtl_Internal::CommandQueue objCObj, Device &device);
    
    cpp_mtl_Internal::CommandQueue objCObj() const;
    
};

#pragma mark - CommandQueue inline method implementations

CPP_METAL_MOVE_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(CommandQueue);

CPP_METAL_OBJCOBJ_GETTER_IMPLEMENATATION(CommandQueue);

inline void CommandQueue::label(const char *string) {
    CPP_METAL_PROCESS_LABEL(string, label);
}

} // namespace MTL

#endif // cpp_mtl_CommandQueue_hpp
