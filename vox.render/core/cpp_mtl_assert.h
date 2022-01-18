//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef AAPLUtilities_h
#define AAPLUtilities_h

#include <assert.h>
#include <CoreFoundation/CoreFoundation.h>

namespace MTL {
static inline void assert(const void *success, CFErrorRef error, const char *format, ...) {
    if (0 == success) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        if (error) {
            CFStringRef description = CFErrorCopyDescription(error);
            puts(CFStringGetCStringPtr(description, kCFStringEncodingASCII));
        }
        
        assert(!"AAPLAsertTriggered");
    }
}

static inline void assert(bool success, CFErrorRef error, const char *format, ...) {
    if (false == success) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        if (error) {
            CFStringRef description = CFErrorCopyDescription(error);
            puts(CFStringGetCStringPtr(description, kCFStringEncodingASCII));
        }
        
        assert(!"AAPLAsertTriggered");
    }
}

static inline void assert(const void *success, const char *format, ...) {
    if (nullptr == success) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        assert(!"AAPLAsertTriggered");
    }
}

static inline void assert(bool success, const char *format, ...) {
    
    if (false == success) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        
        assert(!"AAPLAsertTriggered");
    }
}
}

#endif // AAPLUtilities_h
