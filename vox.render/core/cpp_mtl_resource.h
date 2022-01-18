//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cpp_mtl_Resource_hpp
#define cpp_mtl_Resource_hpp

#include "cpp_mtl_implementation.h"
#include "cpp_mtl_types.h"
#include "cpp_mtl_resource_enum.h"


namespace MTL {
class Device;

class Resource {
public:
    Resource();
    
    Resource(const Resource &rhs);
    
    Resource(Resource &&rhs);
    
    Resource &operator=(const Resource &rhs);
    
    Resource &operator=(Resource &&rhs);
    
    virtual ~Resource();
    
    bool operator==(const Resource &rhs) const;
    
    const char *label() const;
    
    void label(const CFStringRef string);
    
    void label(const char *string);
    
    const Device* device() const;
    
    CPUCacheMode cpuCacheMode() const;
    
    StorageMode storageMode() const;
    
    HazardTrackingMode hazardTrackingMode() const;
    
    ResourceOptions resourceOptions() const;
    
protected:
    Resource(cpp_mtl_Internal::Resource objCObj,
             Device &device);
    
    cpp_mtl_Internal::Resource m_objCObj;
    
    Device *m_device;
};


#pragma mark - Resource inline implementations

CPP_METAL_MOVE_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(Resource);

inline void Resource::label(const char *string) {
    CPP_METAL_PROCESS_LABEL(string, label);
}


} // namespace MTL

#endif /* cpp_mtl_Resource_hpp */
