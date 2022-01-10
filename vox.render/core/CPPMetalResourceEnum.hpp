/*
See LICENSE folder for this sample’s licensing information.

Abstract:
C++ Definitions for Metal resource enum types.
*/

#ifndef CPPMetalResourceEnum_hpp
#define CPPMetalResourceEnum_hpp

#include "CPPMetalTypes.hpp"


namespace MTL
{


typedef enum CPUCacheMode
{
    CPUCacheModeDefaultCache = 0,
    CPUCacheModeWriteCombined = 1,
} CPUCacheMode;

typedef enum StorageMode
{
    StorageModeShared  = 0,
    StorageModeManaged API_AVAILABLE(macos(10.11), macCatalyst(13.0)) API_UNAVAILABLE(ios) = 1,
    StorageModePrivate = 2,
    StorageModeMemoryless = 3,
} StorageMode;

typedef enum HazardTrackingMode
{
    HazardTrackingModeDefault = 0,
    HazardTrackingModeUntracked = 1,
    HazardTrackingModeTracked = 2,
} HazardTrackingMode;

static const uint32_t ResourceCPUCacheModeShift = 0;
static const uint32_t ResourceCPUCacheModeMask = (0xfUL << ResourceCPUCacheModeShift);

static const uint32_t ResourceStorageModeShift = 4;
static const uint32_t ResourceStorageModeMask = (0xfUL << ResourceStorageModeShift);

static const uint32_t  ResourceHazardTrackingModeShift = 8;
static const uint32_t  ResourceHazardTrackingModeMask = (0x3UL << ResourceHazardTrackingModeShift);

typedef enum ResourceOptions
{
    ResourceOptionsDefault = 0,

    ResourceCPUCacheModeDefaultCache  = CPUCacheModeDefaultCache  << ResourceCPUCacheModeShift,
    ResourceCPUCacheModeWriteCombined = CPUCacheModeWriteCombined << ResourceCPUCacheModeShift,

    ResourceStorageModeShared     API_AVAILABLE(macos(10.11), ios(9.0))  = StorageModeShared     << ResourceStorageModeShift,
    ResourceStorageModeManaged    API_AVAILABLE(macos(10.11), macCatalyst(13.0)) API_UNAVAILABLE(ios)   = StorageModeManaged    << ResourceStorageModeShift,
    ResourceStorageModePrivate    API_AVAILABLE(macos(10.11), ios(9.0))  = StorageModePrivate    << ResourceStorageModeShift,
    ResourceStorageModeMemoryless API_AVAILABLE(ios(10.0)) API_UNAVAILABLE(macos, macCatalyst) = StorageModeMemoryless << ResourceStorageModeShift,

    ResourceHazardTrackingModeDefault API_AVAILABLE(macos(10.13), ios(10.0)) = HazardTrackingModeDefault << ResourceHazardTrackingModeShift,
    ResourceHazardTrackingModeUntracked API_AVAILABLE(macos(10.13), ios(10.0)) = HazardTrackingModeUntracked << ResourceHazardTrackingModeShift,
    ResourceHazardTrackingModeTracked API_AVAILABLE(macos(10.15), ios(13.0)) = HazardTrackingModeTracked << ResourceHazardTrackingModeShift,
} ResourceOptions;


} // namespace MTL

#endif // CPPMetalResourceEnum_hpp
