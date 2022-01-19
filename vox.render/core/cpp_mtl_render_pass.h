//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cpp_mtl_RenderPass_hpp
#define cpp_mtl_RenderPass_hpp

#include "cpp_mtl_constants.h"
#include "cpp_mtl_implementation.h"
#include "cpp_mtl_types.h"


namespace MTL {


typedef enum LoadAction {
    LoadActionDontCare = 0,
    LoadActionLoad = 1,
    LoadActionClear = 2,
} LoadAction;

typedef enum StoreAction {
    StoreActionDontCare = 0,
    StoreActionStore = 1,
    StoreActionMultisampleResolve = 2,
    StoreActionStoreAndMultisampleResolve API_AVAILABLE(macos(10.12), ios(10.0)) = 3,
    StoreActionUnknown API_AVAILABLE(macos(10.12), ios(10.0)) = 4,
    StoreActionCustomSampleDepthStore API_AVAILABLE(macos(10.13), ios(11.0)) = 5,
} StoreAction;

struct ClearColor {
    double red;
    double green;
    double blue;
    double alpha;
};

inline ClearColor ClearColorMake(double red, double green, double blue, double alpha) {
    ClearColor color = {red, green, blue, alpha};
    return color;
}

class Texture;


class RenderPassAttachmentDescriptor {
public:
    
    RenderPassAttachmentDescriptor() = delete;
    
    RenderPassAttachmentDescriptor(const RenderPassAttachmentDescriptor &rhs);
    
    RenderPassAttachmentDescriptor &operator=(const RenderPassAttachmentDescriptor &rhs);
    
    virtual ~RenderPassAttachmentDescriptor();
    
    Texture &texture() const;
    
    void texture(Texture &texture);
    
    LoadAction loadAction() const;
    
    void loadAction(LoadAction loadAction);
    
    StoreAction storeAction() const;
    
    void storeAction(StoreAction storeAction);
    
protected:
    
    cpp_mtl_internal::RenderPassAttachmentDescriptor m_objCObj;
    
    Texture *m_texture;
    
    // Only allow construction by derived clases
    explicit RenderPassAttachmentDescriptor(cpp_mtl_internal::RenderPassAttachmentDescriptor objCObj);
    
public: // Public methods for cpp_mtl_ internal implementation
    
    cpp_mtl_internal::RenderPassAttachmentDescriptor objCObj() const;
    
};


class RenderPassDepthAttachmentDescriptor : public RenderPassAttachmentDescriptor {
public:
    
    RenderPassDepthAttachmentDescriptor();
    
    RenderPassDepthAttachmentDescriptor(const RenderPassDepthAttachmentDescriptor &rhs);
    
    CPP_METAL_VIRTUAL ~RenderPassDepthAttachmentDescriptor();
    
    RenderPassDepthAttachmentDescriptor &operator=(const RenderPassDepthAttachmentDescriptor &rhs);
    
    double clearDepth() const;
    
    void clearDepth(double value);
    
private:
    
    cpp_mtl_internal::RenderPassDescriptor m_objCContainer;
    
    // RenderPassDescriptor objects must be able to manipulate their
    //   RenderPassDepthAttachmentDescriptor objects during construction and copy operations
    friend class RenderPassDescriptor;
    
    explicit RenderPassDepthAttachmentDescriptor(cpp_mtl_internal::RenderPassDescriptor objCContainer);
    
    void reinitialize(cpp_mtl_internal::RenderPassDescriptor objCContainer);
    
public: // Public methods for cpp_mtl_ internal implementation
    
    cpp_mtl_internal::RenderPassDepthAttachmentDescriptor objCObj() const;
    
};


class RenderPassStencilAttachmentDescriptor : public RenderPassAttachmentDescriptor {
public:
    
    RenderPassStencilAttachmentDescriptor();
    
    RenderPassStencilAttachmentDescriptor(const RenderPassStencilAttachmentDescriptor &rhs);
    
    CPP_METAL_VIRTUAL ~RenderPassStencilAttachmentDescriptor();
    
    RenderPassStencilAttachmentDescriptor &operator=(const RenderPassStencilAttachmentDescriptor &rhs);
    
    uint32_t clearStencil() const;
    
    void clearStencil(uint32_t value);
    
private:
    
    cpp_mtl_internal::RenderPassDescriptor m_objCContainer;
    
    // RenderPassDescriptor objects must be able to manipulate their
    //   RenderPassStencilAttachmentDescriptor objects during construction and copy operations
    friend class RenderPassDescriptor;
    
    explicit RenderPassStencilAttachmentDescriptor(cpp_mtl_internal::RenderPassDescriptor objCContainer);
    
    void reinitialize(cpp_mtl_internal::RenderPassDescriptor objCContainer);
    
public: // Public methods for cpp_mtl_ internal implementation
    
    cpp_mtl_internal::RenderPassStencilAttachmentDescriptor objCObj() const;
    
};

class RenderPassColorAttachmentDescriptor : public RenderPassAttachmentDescriptor {
public:
    
    RenderPassColorAttachmentDescriptor();
    
    RenderPassColorAttachmentDescriptor(const RenderPassColorAttachmentDescriptor &rhs);
    
    CPP_METAL_VIRTUAL ~RenderPassColorAttachmentDescriptor();
    
    RenderPassColorAttachmentDescriptor &operator=(const RenderPassColorAttachmentDescriptor &rhs);
    
    void clearColor(const ClearColor &value);
    
    ClearColor clearColor() const;
    
private:
    
    cpp_mtl_internal::RenderPassColorAttachmentDescriptorArray m_objCContainer;
    
    UInteger m_arrayIndex;
    
    // RenderPassColorAttachmentDescriptorArray objects must be able to manipulate their
    //   RenderPassColorAttachmentDescriptorArray objects during construction and copy operations
    friend class RenderPassColorAttachmentDescriptorArray;
    
    RenderPassColorAttachmentDescriptor(cpp_mtl_internal::RenderPassColorAttachmentDescriptorArray objCContainer, UInteger arrayIndex);
    
    void reinitialize(cpp_mtl_internal::RenderPassColorAttachmentDescriptorArray objCContainer);
    
public: // Public methods for cpp_mtl_ internal implementation
    
    cpp_mtl_internal::RenderPassColorAttachmentDescriptor objCObj() const;
    
};

class RenderPassColorAttachmentDescriptorArray {
public:
    
    RenderPassColorAttachmentDescriptorArray() = delete;
    
    RenderPassColorAttachmentDescriptorArray(const RenderPassColorAttachmentDescriptorArray &rhs);
    
    RenderPassColorAttachmentDescriptorArray &operator=(const RenderPassColorAttachmentDescriptorArray &rhs);
    
    RenderPassColorAttachmentDescriptor operator[](int i) const;
    
    RenderPassColorAttachmentDescriptor &operator[](int i);
    
private:
    
    cpp_mtl_internal::RenderPassColorAttachmentDescriptorArray m_objCObj;
    
    RenderPassColorAttachmentDescriptor m_colorAttachments[MaxColorAttachments];
    
    // RenderPassDescriptor objects must be able to manipulate their
    //   RenderPassColorAttachmentDescriptorArray objects during construction and copy operations
    friend class RenderPassDescriptor;
    
    explicit RenderPassColorAttachmentDescriptorArray(cpp_mtl_internal::RenderPassColorAttachmentDescriptorArray objCObj);
    
    void reinitialize(cpp_mtl_internal::RenderPassColorAttachmentDescriptorArray objCObj);
    
public: // Public methods for cpp_mtl_ internal implementation
    
    cpp_mtl_internal::RenderPassColorAttachmentDescriptorArray objCObj() const;
    
};

class RenderPassDescriptor {
private:
    
    // cpp_mtl_ Internal Implementation Note:
    // The class must declare the 'm_objCObj' member before the 'colorAttachments',
    // 'depthAttachment', and 'stencilAttachment' members.  The constructor of these members depend
    // on the value of properties in 'm_objCObj'.  C++ executes member constructors in order of
    // declaration (not in order of the initializer list in the contstructor).  So the class must
    // declare 'm_objCObj' first for it to initialize it first and allow these members to depend on
    // its properties.
    cpp_mtl_internal::RenderPassDescriptor m_objCObj;
    
public:
    
    RenderPassDescriptor();
    
    RenderPassDescriptor(const RenderPassDescriptor &rhs);
    
    RenderPassDescriptor &operator=(const RenderPassDescriptor &rhs);
    
    CPP_METAL_VIRTUAL ~RenderPassDescriptor();
    
    RenderPassColorAttachmentDescriptorArray colorAttachments;
    
    RenderPassDepthAttachmentDescriptor depthAttachment;
    
    RenderPassStencilAttachmentDescriptor stencilAttachment;
    
public: // Public methods for cpp_mtl_ internal implementation
    
    explicit RenderPassDescriptor(cpp_mtl_internal::RenderPassDescriptor objCObj);
    
    cpp_mtl_internal::RenderPassDescriptor objCObj() const;
    
};


//==================================================================
#pragma mark - RenderPassAttachmentDescriptor inline implementations

inline cpp_mtl_internal::RenderPassAttachmentDescriptor RenderPassAttachmentDescriptor::objCObj() const {
    return m_objCObj;
}


//=======================================================================
#pragma mark - RenderPassDepthAttachmentDescriptor inline implementations

inline cpp_mtl_internal::RenderPassDepthAttachmentDescriptor RenderPassDepthAttachmentDescriptor::objCObj() const {
    return (cpp_mtl_internal::RenderPassDepthAttachmentDescriptor) m_objCObj;
}


//=========================================================================
#pragma mark - RenderPassStencilAttachmentDescriptor inline implementations

inline cpp_mtl_internal::RenderPassStencilAttachmentDescriptor RenderPassStencilAttachmentDescriptor::objCObj() const {
    return (cpp_mtl_internal::RenderPassStencilAttachmentDescriptor) m_objCObj;
}


//=======================================================================
#pragma mark - RenderPassColorAttachmentDescriptor inline implementations

inline cpp_mtl_internal::RenderPassColorAttachmentDescriptor RenderPassColorAttachmentDescriptor::objCObj() const {
    return (cpp_mtl_internal::RenderPassColorAttachmentDescriptor) m_objCObj;
}


//============================================================================
#pragma mark - RenderPassColorAttachmentDescriptorArray inline implementations

inline RenderPassColorAttachmentDescriptor RenderPassColorAttachmentDescriptorArray::operator[](int i) const {
    return m_colorAttachments[i];
}

inline RenderPassColorAttachmentDescriptor &RenderPassColorAttachmentDescriptorArray::operator[](int i) {
    return m_colorAttachments[i];
}

inline cpp_mtl_internal::RenderPassColorAttachmentDescriptorArray RenderPassColorAttachmentDescriptorArray::objCObj() const {
    return m_objCObj;
}


//========================================================
#pragma mark - RenderPassDescriptor inline implementations

inline cpp_mtl_internal::RenderPassDescriptor RenderPassDescriptor::objCObj() const {
    return m_objCObj;
}


} // namespace MTL

#endif // cpp_mtl_RenderPass_hpp
