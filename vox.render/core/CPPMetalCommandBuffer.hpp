/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for C++ Metal command buffer class wrapper
*/

#ifndef CPPMetalCommandBuffer_hpp
#define CPPMetalCommandBuffer_hpp

#include "CPPMetalImplementation.hpp"
#include "CPPMetalCommandQueue.hpp"
#include "CPPMetalTypes.hpp"
#include "CPPMetalDevice.hpp"


namespace MTL
{


class Device;
class RenderPassDescriptor;
class RenderCommandEncoder;
class Drawable;

struct CommandBufferHandler
{
    virtual void operator()(const MTL::CommandBuffer &) = 0;
    virtual ~CommandBufferHandler();
};

class CommandBuffer
{
public:

    CommandBuffer() = delete;

    CommandBuffer(const CommandBuffer& rhs);

    CommandBuffer(CommandBuffer&& rhs);

    CommandBuffer& operator=(const CommandBuffer& rhs);

    CommandBuffer& operator=(CommandBuffer&& rhs);

    CPP_METAL_VIRTUAL ~CommandBuffer();

    bool operator==(const CommandBuffer & rhs);

    const char *label() const;
    void        label(const char *string);
    void        label(const CFStringRef string);

    Device device() const;

    RenderCommandEncoder renderCommandEncoderWithDescriptor(const RenderPassDescriptor & descriptor) const;

    void commit();

    void presentDrawable(Drawable & drawable);

    void waitUntilCompleted();

    void addCompletedHandler(CommandBufferHandler & completedHandler);

    void addScheduledHandler(CommandBufferHandler & scheduledHandler);

private:

    CPPMetalInternal::CommandBuffer m_objCObj;

    Device *m_device;

public: // Public methods for CPPMetal internal implementation

    CommandBuffer(CPPMetalInternal::CommandBuffer objCObj, Device & device);

};


//========================================================
#pragma mark - CommandBuffer inline method implementations

CPP_METAL_MOVE_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(CommandBuffer);

inline void CommandBuffer::label(const char* string)
{
    CPP_METAL_PROCESS_LABEL(string, label);
}


} // namespace MTL

#endif // CPPMetalCommandBuffer_hpp
