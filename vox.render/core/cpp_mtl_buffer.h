//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cpp_mtl_Buffer_hpp
#define cpp_mtl_Buffer_hpp

#include "cpp_mtl_resource.h"

namespace MTL {
class Buffer : public Resource {
public:
    
    Buffer();
    
    Buffer(const Buffer &rhs);
    
    Buffer(Buffer &&rhs);
    
    Buffer &operator=(const Buffer &rhs);
    
    Buffer &operator=(Buffer &&rhs);
    
    CPP_METAL_VIRTUAL ~Buffer();
    
    UInteger length() const;
    
    void *contents() const;
    
    void didModifyRange(CFRange range) API_AVAILABLE(macos(10.11), macCatalyst(13.0)) API_UNAVAILABLE(ios);
    
private:
    
    void *m_contentsPtr;
    
public: // Public methods for cpp_mtl_ internal implementation
    
    Buffer(cpp_mtl_internal::Buffer objCObj, Device &device);
    
    cpp_mtl_internal::Buffer objCObj() const;
    
};

#pragma mark - Buffer inline method implementations

inline Buffer::Buffer()
: Resource(), m_contentsPtr(nullptr) {
    // Member initialization only
}

inline Buffer::Buffer(Buffer &&rhs)
: Resource(rhs), m_contentsPtr(rhs.m_contentsPtr) {
    rhs.m_objCObj = nullptr;
}

inline Buffer &Buffer::operator=(Buffer &&rhs) {
    Resource::operator=(rhs);
    m_contentsPtr = rhs.m_contentsPtr;
    
    return *this;
}

inline void *Buffer::contents() const {
    return m_contentsPtr;
}

inline cpp_mtl_internal::Buffer Buffer::objCObj() const {
    return (cpp_mtl_internal::Buffer) m_objCObj;
}

} // namespace MTL

#endif // cpp_mtl_Buffer_hpp
