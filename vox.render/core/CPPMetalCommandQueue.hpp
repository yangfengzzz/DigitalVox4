/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for C++ Metal command queue class wrapper
*/

#ifndef CPPMetalCommandQueue_hpp
#define CPPMetalCommandQueue_hpp

#include "CPPMetalImplementation.hpp"
#include "CPPMetalTypes.hpp"
#include "CPPMetalConstants.hpp"


namespace MTL
{


class Device;
class CommandBuffer;

class CommandQueue
{

public:

    CommandQueue();

    CommandQueue(const CommandQueue & rhs);

    CommandQueue(CommandQueue && rhs);

    CommandQueue & operator=(const CommandQueue & rhs);

    CommandQueue & operator=(CommandQueue && rhs);

    CPP_METAL_VIRTUAL ~CommandQueue();

    void endEncoding();

    const char *label() const;
    void        label(const char* string);
    void        label(const CFStringRef string);

    Device device() const;

    CommandBuffer commandBuffer();

private:

    CPPMetalInternal::CommandQueue m_objCObj;

    Device *m_device;

public: // Public methods for CPPMetal internal implementation

    CommandQueue(CPPMetalInternal::CommandQueue objCObj, Device & device);

    CPPMetalInternal::CommandQueue objCObj() const;

};


//=======================================================
#pragma mark - CommandQueue inline method implementations

CPP_METAL_MOVE_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(CommandQueue);

CPP_METAL_OBJCOBJ_GETTER_IMPLEMENATATION(CommandQueue);

inline void CommandQueue::label(const char* string)
{
    CPP_METAL_PROCESS_LABEL(string, label);
}

} // namespace MTL

#endif // CPPMetalCommandQueue_hpp
