/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Implementation of C++ Metal render pass descriptor class wrapper
*/


#include "CPPMetalRenderPass.hpp"
#include "CPPMetalTexture.hpp"
#include "CPPMetalInternalMacros.h"

using namespace MTL;

#pragma mark - Enum Validation

CPP_METAL_VALIDATE_ENUM_ALIAS( LoadActionDontCare );
CPP_METAL_VALIDATE_ENUM_ALIAS( LoadActionLoad );
CPP_METAL_VALIDATE_ENUM_ALIAS( LoadActionClear );

CPP_METAL_VALIDATE_ENUM_ALIAS( StoreActionDontCare );
CPP_METAL_VALIDATE_ENUM_ALIAS( StoreActionStore );
CPP_METAL_VALIDATE_ENUM_ALIAS( StoreActionMultisampleResolve );
CPP_METAL_VALIDATE_ENUM_ALIAS( StoreActionStoreAndMultisampleResolve );
CPP_METAL_VALIDATE_ENUM_ALIAS( StoreActionUnknown );
CPP_METAL_VALIDATE_ENUM_ALIAS( StoreActionCustomSampleDepthStore );



//==================================================================
#pragma mark - RenderPassAttachmentDescriptor method implementations

RenderPassAttachmentDescriptor::RenderPassAttachmentDescriptor
(
    CPPMetalInternal::RenderPassAttachmentDescriptor objCObj
)
: m_objCObj(objCObj)
, m_texture(nullptr)
{
    // Member initialization only
}


RenderPassAttachmentDescriptor::RenderPassAttachmentDescriptor(const RenderPassAttachmentDescriptor & rhs)
: m_objCObj([rhs.m_objCObj copyWithZone:nil])
, m_texture(rhs.m_texture)
{
    // Member initialization only
}

RenderPassAttachmentDescriptor & RenderPassAttachmentDescriptor::operator=(const RenderPassAttachmentDescriptor & rhs)
{
    m_objCObj = [rhs.m_objCObj copyWithZone:nil];
    m_texture = rhs.m_texture;

    return *this;
}


RenderPassAttachmentDescriptor::~RenderPassAttachmentDescriptor()
{
    m_objCObj = nil;
}

void RenderPassAttachmentDescriptor::texture(Texture & texture)
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();

    m_objCObj.texture = texture.objCObj();
    m_texture = &texture;
}

Texture & RenderPassAttachmentDescriptor::texture() 
{
    CPP_METAL_VALIDATE_WRAPPED_NIL();

    return *m_texture;
}

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(RenderPassAttachmentDescriptor, LoadAction, loadAction);

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(RenderPassAttachmentDescriptor, StoreAction, storeAction);


//=======================================================================
#pragma mark - RenderPassDepthAttachmentDescriptor method implementations

inline RenderPassDepthAttachmentDescriptor::RenderPassDepthAttachmentDescriptor
(
    CPPMetalInternal::RenderPassDescriptor objCContainer
)
: RenderPassAttachmentDescriptor(objCContainer.depthAttachment)
, m_objCContainer(objCContainer)
{
    // Member initialization only
}

RenderPassDepthAttachmentDescriptor::~RenderPassDepthAttachmentDescriptor()
{
    m_objCContainer = nil;
}

RenderPassDepthAttachmentDescriptor::RenderPassDepthAttachmentDescriptor()
: RenderPassAttachmentDescriptor([MTLRenderPassDepthAttachmentDescriptor new])
, m_objCContainer(nil)
{
}

RenderPassDepthAttachmentDescriptor::RenderPassDepthAttachmentDescriptor(const RenderPassDepthAttachmentDescriptor & rhs)
: RenderPassAttachmentDescriptor(rhs)
, m_objCContainer(nil)
{
}

RenderPassDepthAttachmentDescriptor & RenderPassDepthAttachmentDescriptor::operator=(const RenderPassDepthAttachmentDescriptor & rhs)
{
    if(m_objCContainer)
    {
        assert([rhs.m_objCObj isKindOfClass:[MTLRenderPassDepthAttachmentDescriptor class]]);

        // depthAttachment property follows copy semantics so this is
        //   similar to [rhs.m_objCObj copyWithZone:nil];
        m_objCContainer.depthAttachment = (MTLRenderPassDepthAttachmentDescriptor*)rhs.m_objCObj;

        m_objCObj = m_objCContainer.depthAttachment;
    }
    else
    {
        m_objCObj = [rhs.m_objCObj copyWithZone:nil];
    }

    return *this;
}

void RenderPassDepthAttachmentDescriptor::reinitialize(CPPMetalInternal::RenderPassDescriptor objCObj)
{
    m_objCContainer = objCObj;
    m_objCObj = m_objCObj;
}

void RenderPassDepthAttachmentDescriptor::clearDepth(double value)
{
    ((MTLRenderPassDepthAttachmentDescriptor*)m_objCObj).clearDepth = value;
}

double RenderPassDepthAttachmentDescriptor::clearDepth() const
{
    return ((MTLRenderPassDepthAttachmentDescriptor*)m_objCObj).clearDepth;
}


//=========================================================================
#pragma mark - RenderPassStencilAttachmentDescriptor method implementations

inline RenderPassStencilAttachmentDescriptor::RenderPassStencilAttachmentDescriptor
(
    CPPMetalInternal::RenderPassDescriptor objCContainer
)
: RenderPassAttachmentDescriptor(objCContainer.stencilAttachment)
, m_objCContainer(objCContainer)
{
    // Member initialization only
}

RenderPassStencilAttachmentDescriptor::~RenderPassStencilAttachmentDescriptor()
{
    m_objCContainer = nil;
}

RenderPassStencilAttachmentDescriptor::RenderPassStencilAttachmentDescriptor()
: RenderPassAttachmentDescriptor([MTLRenderPassStencilAttachmentDescriptor new])
, m_objCContainer(nil)
{
    // Member initialization only
}

RenderPassStencilAttachmentDescriptor::RenderPassStencilAttachmentDescriptor(const RenderPassStencilAttachmentDescriptor & rhs)
: RenderPassAttachmentDescriptor(rhs)
, m_objCContainer(nil)
{
    assert([rhs.m_objCObj isKindOfClass:[MTLRenderPassDepthAttachmentDescriptor class]]);
}

RenderPassStencilAttachmentDescriptor & RenderPassStencilAttachmentDescriptor::operator=(const RenderPassStencilAttachmentDescriptor & rhs)
{
    if(m_objCContainer)
    {
        assert([rhs.m_objCObj isKindOfClass:[MTLRenderPassStencilAttachmentDescriptor class]]);

        // stencilAttachment property follows copy semantics so this is
        //   similar to [rhs.m_objCObj copyWithZone:nil];
        m_objCContainer.stencilAttachment = (MTLRenderPassStencilAttachmentDescriptor*)rhs.m_objCObj;

        m_objCObj = m_objCContainer.stencilAttachment;
    }
    else
    {
        m_objCObj = [rhs.m_objCObj copyWithZone:nil];
    }

    return *this;
}

void RenderPassStencilAttachmentDescriptor::reinitialize(CPPMetalInternal::RenderPassDescriptor objCObj)
{
    m_objCContainer = objCObj;
    m_objCObj = m_objCContainer.stencilAttachment;;
}

void RenderPassStencilAttachmentDescriptor::clearStencil(uint32_t value)
{
    ((MTLRenderPassStencilAttachmentDescriptor*)m_objCObj).clearStencil = value;
}

uint32_t RenderPassStencilAttachmentDescriptor::clearStencil() const
{
    return ((MTLRenderPassStencilAttachmentDescriptor*)m_objCObj).clearStencil;
}


//=======================================================================
#pragma mark - RenderPassColorAttachmentDescriptor method implementations

RenderPassColorAttachmentDescriptor::RenderPassColorAttachmentDescriptor()
: RenderPassAttachmentDescriptor([MTLRenderPassColorAttachmentDescriptor new])
, m_objCContainer(nil)
, m_arrayIndex(UIntegerMax) // Assign impossible value since object is not yet assigned to a container
{
}


RenderPassColorAttachmentDescriptor::RenderPassColorAttachmentDescriptor(CPPMetalInternal::RenderPassColorAttachmentDescriptorArray objCContainer, UInteger arrayIndex)
: RenderPassAttachmentDescriptor(objCContainer[arrayIndex])
, m_objCContainer(objCContainer)
, m_arrayIndex(arrayIndex)
{

}


RenderPassColorAttachmentDescriptor::RenderPassColorAttachmentDescriptor(const RenderPassColorAttachmentDescriptor & rhs) :
RenderPassAttachmentDescriptor(rhs),
m_objCContainer(nil),
m_arrayIndex(MaxColorAttachments)
{
}


RenderPassColorAttachmentDescriptor::~RenderPassColorAttachmentDescriptor()
{
    m_objCObj = nil;
    m_objCContainer = nil;
    m_arrayIndex = MaxColorAttachments;
}


RenderPassColorAttachmentDescriptor & RenderPassColorAttachmentDescriptor::operator=(const RenderPassColorAttachmentDescriptor & rhs)
{
    if(m_objCContainer)
    {
        assert([rhs.m_objCObj isKindOfClass:[MTLRenderPassColorAttachmentDescriptor class]]);

        m_objCContainer[m_arrayIndex] = (MTLRenderPassColorAttachmentDescriptor*)rhs.m_objCObj;
    }
    else
    {
        m_objCObj = [rhs.m_objCObj copyWithZone:nil];
    }

    return *this;
}

void RenderPassColorAttachmentDescriptor::reinitialize(CPPMetalInternal::RenderPassColorAttachmentDescriptorArray objCContainer)
{
    m_objCObj = objCContainer[m_arrayIndex];
    m_objCContainer = objCContainer;
}

void RenderPassColorAttachmentDescriptor::clearColor(const ClearColor & color)
{
    ((MTLRenderPassColorAttachmentDescriptor*)m_objCObj).clearColor =
        MTLClearColorMake(color.red, color.green, color.blue, color.alpha);
}

ClearColor RenderPassColorAttachmentDescriptor::clearColor() const
{
    return ClearColorMake(((MTLRenderPassColorAttachmentDescriptor*)m_objCObj).clearColor.red,
                          ((MTLRenderPassColorAttachmentDescriptor*)m_objCObj).clearColor.green,
                          ((MTLRenderPassColorAttachmentDescriptor*)m_objCObj).clearColor.blue,
                          ((MTLRenderPassColorAttachmentDescriptor*)m_objCObj).clearColor.alpha);
}


//============================================================================
#pragma mark - RenderPassColorAttachmentDescriptorArray method implementations

RenderPassColorAttachmentDescriptorArray::RenderPassColorAttachmentDescriptorArray(CPPMetalInternal::RenderPassColorAttachmentDescriptorArray objCObj) :
m_objCObj(objCObj),
m_colorAttachments {
    {objCObj, 0},
    {objCObj, 1},
    {objCObj, 2},
    {objCObj, 3},
    {objCObj, 4},
    {objCObj, 5},
    {objCObj, 6},
    {objCObj, 7}
}
{
    // Validate that all attachments have been set in the correct index
    for(int i = 0; i < MaxColorAttachments; i++)
    {
        assert(m_colorAttachments[i].objCObj() == objCObj[i]);
    }
}


void RenderPassColorAttachmentDescriptorArray::reinitialize(CPPMetalInternal::RenderPassColorAttachmentDescriptorArray objCObj)
{
    m_objCObj = objCObj;

    for(int i = 0; i < MaxColorAttachments; i++)
    {
        m_colorAttachments[i].reinitialize(m_objCObj);
    }
}


//========================================================
#pragma mark - RenderPassDescriptor method implementations

RenderPassDescriptor::RenderPassDescriptor()
: m_objCObj([MTLRenderPassDescriptor new])
, depthAttachment(m_objCObj)
, stencilAttachment(m_objCObj)
, colorAttachments(m_objCObj.colorAttachments)
{
    assert(m_objCObj.stencilAttachment == stencilAttachment.objCObj());
    assert(m_objCObj.depthAttachment == depthAttachment.objCObj());
}

RenderPassDescriptor::RenderPassDescriptor(CPPMetalInternal::RenderPassDescriptor objCObj)
: m_objCObj(objCObj)
, depthAttachment(objCObj)
, stencilAttachment(objCObj)
, colorAttachments(objCObj.colorAttachments)
{
    assert(m_objCObj.stencilAttachment == stencilAttachment.objCObj());
    assert(m_objCObj.depthAttachment == depthAttachment.objCObj());
}

RenderPassDescriptor::RenderPassDescriptor(const RenderPassDescriptor & rhs)
: m_objCObj([rhs.m_objCObj copyWithZone:nil])
, depthAttachment(m_objCObj)
, stencilAttachment(m_objCObj)
, colorAttachments(m_objCObj.colorAttachments)
{
    assert(m_objCObj.stencilAttachment == stencilAttachment.objCObj());
    assert(m_objCObj.depthAttachment == depthAttachment.objCObj());
}

RenderPassDescriptor & RenderPassDescriptor::operator=(const RenderPassDescriptor & rhs)
{
    m_objCObj = [rhs.m_objCObj copyWithZone:nil];

    colorAttachments.reinitialize(m_objCObj.colorAttachments);

    stencilAttachment.reinitialize(m_objCObj);

    depthAttachment.reinitialize(m_objCObj);

    return *this;
}

RenderPassDescriptor::~RenderPassDescriptor()
{
    m_objCObj = nil;
}
