/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for C++ Metal render pass descriptor class wrappers
*/

#ifndef CPPMetalRenderPass_hpp
#define CPPMetalRenderPass_hpp

#include "CPPMetalConstants.hpp"
#include "CPPMetalImplementation.hpp"
#include "CPPMetalTypes.hpp"


namespace MTL
{


typedef enum LoadAction
{
    LoadActionDontCare = 0,
    LoadActionLoad = 1,
    LoadActionClear = 2,
} LoadAction;

typedef enum StoreAction
{
    StoreActionDontCare = 0,
    StoreActionStore = 1,
    StoreActionMultisampleResolve = 2,
    StoreActionStoreAndMultisampleResolve API_AVAILABLE(macos(10.12), ios(10.0)) = 3,
    StoreActionUnknown API_AVAILABLE(macos(10.12), ios(10.0)) = 4,
    StoreActionCustomSampleDepthStore API_AVAILABLE(macos(10.13), ios(11.0)) = 5,
} StoreAction;

struct ClearColor
{
    double red;
    double green;
    double blue;
    double alpha;
};

inline ClearColor ClearColorMake(double red, double green, double blue, double alpha)
{
    ClearColor color = { red, green, blue, alpha };
    return color;
}

class Texture;


class RenderPassAttachmentDescriptor
{
public:

    RenderPassAttachmentDescriptor() = delete;

    RenderPassAttachmentDescriptor(const RenderPassAttachmentDescriptor & rhs);

    RenderPassAttachmentDescriptor & operator=(const RenderPassAttachmentDescriptor & rhs);

    virtual ~RenderPassAttachmentDescriptor();

    Texture & texture();
    void      texture(Texture & texture);

    LoadAction loadAction() const;
    void       loadAction(LoadAction loadAction);

    StoreAction storeAction() const;
    void        storeAction(StoreAction storeAction);

protected:

    CPPMetalInternal::RenderPassAttachmentDescriptor m_objCObj;

    Texture *m_texture;

    // Only allow construction by derived clases
    explicit RenderPassAttachmentDescriptor(CPPMetalInternal::RenderPassAttachmentDescriptor objCObj);

public: // Public methods for CPPMetal internal implementation

    CPPMetalInternal::RenderPassAttachmentDescriptor objCObj() const;

};


class RenderPassDepthAttachmentDescriptor : public RenderPassAttachmentDescriptor
{
public:

    RenderPassDepthAttachmentDescriptor();

    RenderPassDepthAttachmentDescriptor(const RenderPassDepthAttachmentDescriptor & rhs);

    CPP_METAL_VIRTUAL ~RenderPassDepthAttachmentDescriptor();

    RenderPassDepthAttachmentDescriptor & operator=(const RenderPassDepthAttachmentDescriptor & rhs);

    double clearDepth() const;
    void   clearDepth(double value);

private:

    CPPMetalInternal::RenderPassDescriptor m_objCContainer;

    // RenderPassDescriptor objects must be able to manipulate their
    //   RenderPassDepthAttachmentDescriptor objects during construction and copy operations
    friend class RenderPassDescriptor;

    explicit RenderPassDepthAttachmentDescriptor(CPPMetalInternal::RenderPassDescriptor objCContainer);

    void reinitialize(CPPMetalInternal::RenderPassDescriptor objCContainer);

public: // Public methods for CPPMetal internal implementation

    CPPMetalInternal::RenderPassDepthAttachmentDescriptor objCObj() const;

};


class RenderPassStencilAttachmentDescriptor : public RenderPassAttachmentDescriptor
{
public:

    RenderPassStencilAttachmentDescriptor();

    RenderPassStencilAttachmentDescriptor(const RenderPassStencilAttachmentDescriptor & rhs);

    CPP_METAL_VIRTUAL ~RenderPassStencilAttachmentDescriptor();

    RenderPassStencilAttachmentDescriptor & operator=(const RenderPassStencilAttachmentDescriptor & rhs);

    uint32_t clearStencil() const;
    void     clearStencil(uint32_t value);

private:

    CPPMetalInternal::RenderPassDescriptor m_objCContainer;

    // RenderPassDescriptor objects must be able to manipulate their
    //   RenderPassStencilAttachmentDescriptor objects during construction and copy operations
    friend class RenderPassDescriptor;

    explicit RenderPassStencilAttachmentDescriptor(CPPMetalInternal::RenderPassDescriptor objCContainer);

    void reinitialize(CPPMetalInternal::RenderPassDescriptor objCContainer);

public: // Public methods for CPPMetal internal implementation

    CPPMetalInternal::RenderPassStencilAttachmentDescriptor objCObj() const;

};

class RenderPassColorAttachmentDescriptor : public RenderPassAttachmentDescriptor
{
public:

    RenderPassColorAttachmentDescriptor();

    RenderPassColorAttachmentDescriptor(const RenderPassColorAttachmentDescriptor & rhs);

    CPP_METAL_VIRTUAL ~RenderPassColorAttachmentDescriptor();

    RenderPassColorAttachmentDescriptor & operator=(const RenderPassColorAttachmentDescriptor & rhs);

    void clearColor(const ClearColor & value);
    ClearColor clearColor() const;

private:

    CPPMetalInternal::RenderPassColorAttachmentDescriptorArray m_objCContainer;

    UInteger m_arrayIndex;

    // RenderPassColorAttachmentDescriptorArray objects must be able to manipulate their
    //   RenderPassColorAttachmentDescriptorArray objects during construction and copy operations
    friend class RenderPassColorAttachmentDescriptorArray;

    RenderPassColorAttachmentDescriptor(CPPMetalInternal::RenderPassColorAttachmentDescriptorArray objCContainer, UInteger arrayIndex);

    void reinitialize(CPPMetalInternal::RenderPassColorAttachmentDescriptorArray objCContainer);

public: // Public methods for CPPMetal internal implementation

    CPPMetalInternal::RenderPassColorAttachmentDescriptor objCObj() const;

};

class RenderPassColorAttachmentDescriptorArray
{
public:

    RenderPassColorAttachmentDescriptorArray() = delete;

    RenderPassColorAttachmentDescriptorArray(const RenderPassColorAttachmentDescriptorArray & rhs);

    RenderPassColorAttachmentDescriptorArray & operator=(const RenderPassColorAttachmentDescriptorArray & rhs);

    RenderPassColorAttachmentDescriptor operator[](int i) const;

    RenderPassColorAttachmentDescriptor & operator[](int i);

private:

    CPPMetalInternal::RenderPassColorAttachmentDescriptorArray m_objCObj;

    RenderPassColorAttachmentDescriptor m_colorAttachments[MaxColorAttachments];

    // RenderPassDescriptor objects must be able to manipulate their
    //   RenderPassColorAttachmentDescriptorArray objects during construction and copy operations
    friend class RenderPassDescriptor;

    explicit RenderPassColorAttachmentDescriptorArray(CPPMetalInternal::RenderPassColorAttachmentDescriptorArray objCObj);

    void reinitialize(CPPMetalInternal::RenderPassColorAttachmentDescriptorArray objCObj);

public: // Public methods for CPPMetal internal implementation

    CPPMetalInternal::RenderPassColorAttachmentDescriptorArray objCObj() const;

};

class RenderPassDescriptor
{
private:

    // CPPMetal Internal Implementation Note:
    // The class must declare the 'm_objCObj' member before the 'colorAttachments',
    // 'depthAttachment', and 'stencilAttachment' members.  The constructor of these members depend
    // on the value of properties in 'm_objCObj'.  C++ executes member constructors in order of
    // declaration (not in order of the initializer list in the contstructor).  So the class must
    // declare 'm_objCObj' first for it to initialize it first and allow these members to depend on
    // its properties.
    CPPMetalInternal::RenderPassDescriptor m_objCObj;

public:

    RenderPassDescriptor();

    RenderPassDescriptor(const RenderPassDescriptor & rhs);

    RenderPassDescriptor & operator=(const RenderPassDescriptor & rhs);

    CPP_METAL_VIRTUAL ~RenderPassDescriptor();

    RenderPassColorAttachmentDescriptorArray colorAttachments;

    RenderPassDepthAttachmentDescriptor depthAttachment;

    RenderPassStencilAttachmentDescriptor stencilAttachment;

public: // Public methods for CPPMetal internal implementation

    explicit RenderPassDescriptor(CPPMetalInternal::RenderPassDescriptor objCObj);

    CPPMetalInternal::RenderPassDescriptor objCObj() const;

};


//==================================================================
#pragma mark - RenderPassAttachmentDescriptor inline implementations

inline CPPMetalInternal::RenderPassAttachmentDescriptor RenderPassAttachmentDescriptor::objCObj() const
{
    return m_objCObj;
}


//=======================================================================
#pragma mark - RenderPassDepthAttachmentDescriptor inline implementations

inline CPPMetalInternal::RenderPassDepthAttachmentDescriptor RenderPassDepthAttachmentDescriptor::objCObj() const
{
    return (CPPMetalInternal::RenderPassDepthAttachmentDescriptor)m_objCObj;
}


//=========================================================================
#pragma mark - RenderPassStencilAttachmentDescriptor inline implementations

inline CPPMetalInternal::RenderPassStencilAttachmentDescriptor RenderPassStencilAttachmentDescriptor::objCObj() const
{
    return (CPPMetalInternal::RenderPassStencilAttachmentDescriptor)m_objCObj;
}


//=======================================================================
#pragma mark - RenderPassColorAttachmentDescriptor inline implementations

inline CPPMetalInternal::RenderPassColorAttachmentDescriptor RenderPassColorAttachmentDescriptor::objCObj() const
{
    return (CPPMetalInternal::RenderPassColorAttachmentDescriptor)m_objCObj;
}


//============================================================================
#pragma mark - RenderPassColorAttachmentDescriptorArray inline implementations

inline RenderPassColorAttachmentDescriptor RenderPassColorAttachmentDescriptorArray::operator[](int i) const
{
    return m_colorAttachments[i];
}

inline RenderPassColorAttachmentDescriptor & RenderPassColorAttachmentDescriptorArray::operator[](int i)
{
    return m_colorAttachments[i];
}

inline CPPMetalInternal::RenderPassColorAttachmentDescriptorArray RenderPassColorAttachmentDescriptorArray::objCObj() const
{
    return m_objCObj;
}


//========================================================
#pragma mark - RenderPassDescriptor inline implementations

inline CPPMetalInternal::RenderPassDescriptor RenderPassDescriptor::objCObj() const
{
    return m_objCObj;
}


} // namespace MTL

#endif // CPPMetalRenderPass_hpp
