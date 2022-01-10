/*
 See LICENSE folder for this sample’s licensing information.
 
 Abstract:
 Implementation of C++ Metal resource wrapper
 */


#include "CPPMetalResource.hpp"
#include "CPPMetalDevice.hpp"
#include "CPPMetalInternalMacros.h"

using namespace MTL;

#pragma mark - Enum Validation

CPP_METAL_VALIDATE_ENUM_ALIAS(CPUCacheModeDefaultCache);
CPP_METAL_VALIDATE_ENUM_ALIAS(CPUCacheModeWriteCombined);
CPP_METAL_VALIDATE_ENUM_ALIAS(StorageModeShared);
#if TARGET_OS_OSX
CPP_METAL_VALIDATE_ENUM_ALIAS(StorageModeManaged);
#endif
CPP_METAL_VALIDATE_ENUM_ALIAS(StorageModePrivate);
#if !TARGET_OS_OSX
CPP_METAL_VALIDATE_ENUM_ALIAS( StorageModeMemoryless );
#endif

CPP_METAL_VALIDATE_ENUM_ALIAS(HazardTrackingModeDefault);
CPP_METAL_VALIDATE_ENUM_ALIAS(HazardTrackingModeUntracked);
CPP_METAL_VALIDATE_ENUM_ALIAS(HazardTrackingModeTracked);

CPP_METAL_VALIDATE_ENUM_ALIAS(ResourceCPUCacheModeShift);
CPP_METAL_VALIDATE_ENUM_ALIAS(ResourceCPUCacheModeMask);
CPP_METAL_VALIDATE_ENUM_ALIAS(ResourceStorageModeShift);
CPP_METAL_VALIDATE_ENUM_ALIAS(ResourceStorageModeMask);
CPP_METAL_VALIDATE_ENUM_ALIAS(ResourceHazardTrackingModeShift);
CPP_METAL_VALIDATE_ENUM_ALIAS(ResourceHazardTrackingModeMask);
CPP_METAL_VALIDATE_ENUM_ALIAS(ResourceCPUCacheModeDefaultCache);
CPP_METAL_VALIDATE_ENUM_ALIAS(ResourceCPUCacheModeWriteCombined);
CPP_METAL_VALIDATE_ENUM_ALIAS(ResourceStorageModeShared);
#if TARGET_OS_OSX
CPP_METAL_VALIDATE_ENUM_ALIAS(ResourceStorageModeManaged);
#endif
CPP_METAL_VALIDATE_ENUM_ALIAS(ResourceStorageModePrivate);
#if !TARGET_OS_OSX
CPP_METAL_VALIDATE_ENUM_ALIAS( ResourceStorageModeMemoryless );
#endif
CPP_METAL_VALIDATE_ENUM_ALIAS(ResourceHazardTrackingModeDefault);
CPP_METAL_VALIDATE_ENUM_ALIAS(ResourceHazardTrackingModeUntracked);
CPP_METAL_VALIDATE_ENUM_ALIAS(ResourceHazardTrackingModeTracked);

#pragma mark - RenderPipelineColorAttachmentDescriptor

CPP_METAL_NULL_REFERENCE_CONSTRUCTOR_IMPLEMENATATION(Resource);

CPP_METAL_CONSTRUCTOR_IMPLEMENTATION(Resource);

Resource::Resource(const Resource &rhs)
: m_objCObj(rhs.m_objCObj), m_device(rhs.m_device) {
    
}

Resource &Resource::operator=(const Resource &rhs) {
    m_objCObj = rhs.m_objCObj;
    m_device = rhs.m_device;
    return *this;
}

//CPP_METAL_COPY_CONSTRUCTOR_AND_OPERATOR_OVERLOAD_IMPLEMENTATION(Resource);

Resource::~Resource() {
    m_objCObj = nil;
}

bool Resource::operator==(const Resource &rhs) const {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    return [m_objCObj isEqual:rhs.m_objCObj];
}

CPUCacheMode Resource::cpuCacheMode() const {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    return (CPUCacheMode) m_objCObj.resourceOptions;
}

StorageMode Resource::storageMode() const {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    return (StorageMode) m_objCObj.storageMode;
}

HazardTrackingMode Resource::hazardTrackingMode() const {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    return (HazardTrackingMode) m_objCObj.hazardTrackingMode;
}

ResourceOptions Resource::resourceOptions() const {
    CPP_METAL_VALIDATE_WRAPPED_NIL();
    
    return (ResourceOptions) m_objCObj.resourceOptions;
}

CPP_METAL_READWRITE_LABEL_PROPERTY_IMPLEMENTATION(Resource);

CPP_METAL_DEVICE_GETTER_IMPLEMENTATION(Resource);
