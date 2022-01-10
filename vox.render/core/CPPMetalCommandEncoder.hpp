/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for C++ Metal command encoder class wrapper
*/

#ifndef CPPMetalCommandEncoeder_happ
#define CPPMetalCommandEncoeder_happ

#include "CPPMetalImplementation.hpp"
#include "CPPMetalPixelFormat.hpp"
#include "CPPMetalTypes.hpp"
#include "CPPMetalDevice.hpp"


namespace MTL
{


class Device;

class CommandEncoder
{
public:

    CommandEncoder() = delete;

    CommandEncoder(const CommandEncoder & rhs);

    CommandEncoder(CommandEncoder && rhs);

    CommandEncoder & operator=(const CommandEncoder & rhs);

    CommandEncoder & operator=(CommandEncoder && rhs);

    virtual ~CommandEncoder();

    void endEncoding();

    const char* label() const;
    void        label(const char* string);
    void        label(const CFStringRef string);

    void pushDebugGroup(const char* string);
    void pushDebugGroup(const CFStringRef string);
    void popDebugGroup();

    Device device() const;

protected:

    CommandEncoder(CPPMetalInternal::CommandEncoder objCObj, Device & device);

    Device *m_device;

    CPPMetalInternal::CommandEncoder m_objCObj;

};

//=========================================================
#pragma mark - CommandEncoder inline method implementations

CPP_METAL_MOVE_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(CommandEncoder);

inline void CommandEncoder::pushDebugGroup(const char* string)
{
    CPP_METAL_PROCESS_LABEL(string, pushDebugGroup);
}

inline void CommandEncoder::label(const char* string)
{
    CPP_METAL_PROCESS_LABEL(string, label);
}


} // namespace MTL

#endif // CPPMetalCommandEncoeder_hpp
