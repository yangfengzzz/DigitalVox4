/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for C++ Metal resource class wrapper
*/

#ifndef CPPMetalResource_hpp
#define CPPMetalResource_hpp

#include "CPPMetalImplementation.hpp"
#include "CPPMetalTypes.hpp"
#include "CPPMetalResourceEnum.hpp"


namespace MTL
{


class Device;

class Resource
{
public:

    Resource();

    Resource(const Resource & rhs);

    Resource(Resource && rhs);

    Resource & operator=(const Resource & rhs);

    Resource & operator=(Resource && rhs);

    virtual ~Resource();

    bool operator==(const Resource & rhs) const;

    const char* label() const;
    void        label(const CFStringRef string);
    void        label(const char* string);

    Device device() const;

    CPUCacheMode cpuCacheMode() const;

    StorageMode storageMode() const;

    HazardTrackingMode hazardTrackingMode() const;

    ResourceOptions resourceOptions() const;

protected:

    Resource(CPPMetalInternal::Resource objCObj,
             Device & device);

    CPPMetalInternal::Resource m_objCObj;

    Device *m_device;

};


//============================================
#pragma mark - Resource inline implementations

CPP_METAL_MOVE_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(Resource);

inline void Resource::label(const char* string)
{
    CPP_METAL_PROCESS_LABEL(string, label);
}


} // namespace MTL

#endif /* CPPMetalResource_hpp */
