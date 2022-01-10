/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for C++ Metal buffer class wrapper
*/

#ifndef CPPMetalBuffer_hpp
#define CPPMetalBuffer_hpp

#include "CPPMetalResource.hpp"


namespace MTL
{

class Buffer : public Resource
{
public:

    Buffer();

    Buffer(const Buffer & rhs);

    Buffer(Buffer && rhs);

    Buffer & operator=(const Buffer & rhs);

    Buffer & operator=(Buffer && rhs);

    CPP_METAL_VIRTUAL ~Buffer();

    UInteger length() const;

    void *contents() const;

    void didModifyRange(CFRange range) API_AVAILABLE(macos(10.11), macCatalyst(13.0)) API_UNAVAILABLE(ios);

private:

    void *m_contentsPtr;

public: // Public methods for CPPMetal internal implementation

    Buffer(CPPMetalInternal::Buffer objCObj, Device & device);

    CPPMetalInternal::Buffer objCObj() const;

};


//=================================================
#pragma mark - Buffer inline method implementations

inline Buffer::Buffer()
: Resource()
, m_contentsPtr(nullptr)
{
    // Member initialization only
}

inline Buffer::Buffer(Buffer && rhs)
: Resource(rhs)
, m_contentsPtr(rhs.m_contentsPtr)
{
    rhs.m_objCObj = nullptr;
}

inline Buffer & Buffer::operator=(Buffer && rhs)
{
    Resource::operator=(rhs);
    m_contentsPtr = rhs.m_contentsPtr;

    return *this;
}

inline void *Buffer::contents() const
{
    return m_contentsPtr;
}

inline CPPMetalInternal::Buffer Buffer::objCObj() const
{
    return (CPPMetalInternal::Buffer)m_objCObj;
}


} // namespace MTL

#endif // CPPMetalBuffer_hpp
