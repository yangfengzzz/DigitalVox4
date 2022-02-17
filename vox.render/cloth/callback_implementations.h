//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.


#ifndef callback_implementations_hpp
#define callback_implementations_hpp

#include <NvCloth/Callbacks.h>
#include <foundation/PxAllocatorCallback.h>
#include <foundation/PxErrorCallback.h>
#include <foundation/PxProfiler.h>

#include <vector>
#include <map>

#include <string>
#include <sstream>
#include <assert.h>
#include <mutex>

#include <NvCloth/DxContextManagerCallback.h>
#include <NvCloth/Callbacks.h>

#ifdef _MSC_VER
#include <Windows.h>
#endif

namespace vox {
namespace cloth {
class Allocator : public physx::PxAllocatorCallback {
public:
    Allocator() {
        mEnableLeakDetection = false;
    }
    
    virtual void *allocate(size_t size, const char *typeName, const char *filename, int line) {
#ifdef _MSC_VER
        void* ptr = _aligned_malloc(size, 16);
#else
        void *ptr;
        if (posix_memalign(&ptr, 16, size)) ptr = 0;
#endif
        if (mEnableLeakDetection) {
            std::lock_guard<std::mutex> lock(mAllocationsMapLock);
            mAllocations[ptr] = Allocation(size, typeName, filename, line);
        }
        return ptr;
    }
    
    virtual void deallocate(void *ptr) {
        if (mEnableLeakDetection && ptr) {
            std::lock_guard<std::mutex> lock(mAllocationsMapLock);
            auto i = mAllocations.find(ptr);
            if (i == mAllocations.end()) {
                printf("Tried to deallocate %p which was not allocated with this allocator callback.", ptr);
            } else {
                mAllocations.erase(i);
            }
        }
#ifdef _MSC_VER
        _aligned_free(ptr);
#else
        free(ptr);
#endif
    }
    
    void StartTrackingLeaks() {
        std::lock_guard<std::mutex> lock(mAllocationsMapLock);
        mAllocations.clear();
        mEnableLeakDetection = true;
    }
    
    void StopTrackingLeaksAndReport() {
        std::lock_guard<std::mutex> lock(mAllocationsMapLock);
        mEnableLeakDetection = false;
        
        size_t totalBytes = 0;
        std::stringstream message;
        message << "Memory leaks detected:\n";
        for (auto it = mAllocations.begin(); it != mAllocations.end(); ++it) {
            const Allocation &alloc = it->second;
            message << "* Allocated ptr " << it->first << " of " << alloc.mSize << "bytes (type="
            << alloc.mTypeName << ") at " << alloc.mFileName << ":" << alloc.mLine << "\n";
            totalBytes += alloc.mSize;
        }
        if (mAllocations.size() > 0) {
            message << "=====Total of " << totalBytes << " bytes in " << mAllocations.size()
            << " allocations leaked=====";
            const std::string &tmp = message.str();
#ifdef _MSC_VER
            // OutputDebugString(tmp.c_str()); //Write to visual studio output so we can see it after the application closes
#endif
            printf("%s\n", tmp.c_str());
        }
        
        mAllocations.clear();
    }
    
private:
    bool mEnableLeakDetection;
    
    struct Allocation {
        Allocation() {
        }
        
        Allocation(size_t size, const char *typeName, const char *filename, int line)
        : mSize(size), mTypeName(typeName), mFileName(filename), mLine(line) {
            
        }
        
        size_t mSize;
        std::string mTypeName;
        std::string mFileName;
        int mLine;
    };
    
    std::map<void *, Allocation> mAllocations;
    std::mutex mAllocationsMapLock;
};

class ErrorCallback : public physx::PxErrorCallback {
public:
    ErrorCallback() {
    }
    
    virtual void reportError(physx::PxErrorCode::Enum code, const char *message, const char *file, int line);
};

class NvClothEnvironment {
    NvClothEnvironment() {
        SetUp();
    }
    
    virtual ~NvClothEnvironment() {
        TearDown();
    }
    
    static NvClothEnvironment *sEnv;
    
public:
    static void AllocateEnv() {
        sEnv = new NvClothEnvironment;
    }
    
    static void FreeEnv() {
        delete sEnv;
        sEnv = nullptr;
    }
    
    static void ReportEnvFreed() {
        sEnv = nullptr;
    } //google test will free it for us, so we just reset the value
    static NvClothEnvironment *GetEnv() {
        return sEnv;
    }
    
    virtual void SetUp() {
        mAllocator = new Allocator;
        mAllocator->StartTrackingLeaks();
        mFoundationAllocator = new Allocator;
        mFoundationAllocator->StartTrackingLeaks();
        mErrorCallback = new ErrorCallback;
        nv::cloth::InitializeNvCloth(mAllocator, mErrorCallback, nullptr, nullptr);
    }
    
    virtual void TearDown() {
        mAllocator->StopTrackingLeaksAndReport();
        mFoundationAllocator->StopTrackingLeaksAndReport();
        delete mErrorCallback;
        delete mFoundationAllocator;
        delete mAllocator;
    }
    
    Allocator *GetAllocator() {
        return mAllocator;
    }
    
    Allocator *GetFoundationAllocator() {
        return mFoundationAllocator;
    }
    
    ErrorCallback *GetErrorCallback() {
        return mErrorCallback;
    }
    
private:
    Allocator *mAllocator;
    Allocator *mFoundationAllocator;
    ErrorCallback *mErrorCallback;
};

}
}

#endif /* callback_implementations_hpp */
