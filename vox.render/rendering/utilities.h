/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for renderer class which performs Metal setup and per frame rendering
*/
#ifndef AAPLUtilities_h
#define AAPLUtilities_h

#include <assert.h>
#include <CoreFoundation/CoreFoundation.h>

static inline void AAPLAssert(const void * success, CFErrorRef error, const char* format, ...)
{
    if(0 == success)
    {
        va_list args;
        va_start( args, format );
        vprintf( format, args );
        va_end( args );
        if(error)
        {
            CFStringRef description = CFErrorCopyDescription(error);
            puts(CFStringGetCStringPtr(description, kCFStringEncodingASCII));
        }

        assert(!"AAPLAsertTriggered");
    }
}


static inline void AAPLAssert(bool success, CFErrorRef error, const char* format, ...)
{
    if(false == success)
    {
        va_list args;
        va_start( args, format );
        vprintf( format, args );
        va_end( args );
        if(error)
        {
            CFStringRef description = CFErrorCopyDescription(error);
            puts(CFStringGetCStringPtr(description, kCFStringEncodingASCII));
        }

        assert(!"AAPLAsertTriggered");
    }
}

static inline void AAPLAssert(const void * success, const char* format, ...)
{
    if(nullptr == success)
    {
        va_list args;
        va_start(args, format);
        vprintf( format, args );
        va_end(args);
        assert(!"AAPLAsertTriggered");
    }
}

static inline void AAPLAssert(bool success, const char* format, ...)
{

    if(false == success)
    {
        va_list args;
        va_start(args, format);
        vprintf( format, args );
        va_end(args);

        assert(!"AAPLAsertTriggered");
    }
}

#endif // AAPLUtilities_h
