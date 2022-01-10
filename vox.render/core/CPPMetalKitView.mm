/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Implementation of C++ MetalKit view class wrapper
*/

#include <Metal/Metal.h>
#include "CPPMetalKitView.hpp"
#include "CPPMetalInternalMacros.h"
#include "CPPMetalDevice.hpp"
#include "CPPMetalDrawable.hpp"
#include "CPPMetalTexture.hpp"
#include "CPPMetalRenderPass.hpp"
#include "CPPMetalDeviceInternals.h"


using namespace MTL;
using namespace MTK;

View::View(CPPMetalInternal::View objCObj, MTL::Device & device) :
m_objCObj(objCObj),
m_device(&device),
m_currentDrawable(nullptr),
m_currentRenderPassDescriptor(nullptr),
m_depthStencilTexture(nullptr)
{
    m_objCObj.device = device.objCObj();
}

View::View(const View & rhs) :
m_objCObj(rhs.m_objCObj),
m_device(rhs.m_device),
m_currentDrawable(nullptr),
m_currentRenderPassDescriptor(nullptr),
m_depthStencilTexture(nullptr)
{
    if(rhs.m_depthStencilTexture)
    {
        m_depthStencilTexture = construct<Texture>(m_device->allocator(), *rhs.m_depthStencilTexture);
    }
}

View::~View()
{
    m_objCObj = nil;
    destroy(m_device->allocator(), m_currentDrawable);
    destroy(m_device->allocator(), m_currentRenderPassDescriptor);
    destroy(m_device->allocator(), m_depthStencilTexture);
}

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(View, PixelFormat, depthStencilPixelFormat);

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(View, PixelFormat, colorPixelFormat);

CPP_METAL_READWRITE_BOOL_PROPERTY_IMPLEMENTATION(View, paused, isPaused);

CPP_METAL_READWRITE_BOOL_PROPERTY_IMPLEMENTATION(View, hidden, isHidden);

Device & View::device()
{
    return *m_device;
}

void View::draw()
{
    [m_objCObj draw];
}

Texture *View::depthStencilTexture()
{
    if(!m_objCObj.depthStencilTexture)
    {
        return nil;
    }

    if(!m_depthStencilTexture ||
       ![m_depthStencilTexture->objCObj() isEqual:m_objCObj.depthStencilTexture])
    {
        destroy(m_device->allocator(), m_depthStencilTexture);

        m_depthStencilTexture = construct<Texture>(m_device->allocator(),
                                                   m_objCObj.depthStencilTexture,
                                                   *m_device);
    }

    return m_depthStencilTexture;
}

MTL::Size View::drawableSize() const
{
    MTL::Size size = SizeMake(m_objCObj.drawableSize.width, m_objCObj.drawableSize.height, 0);
    return size;
}

inline  Drawable *View::currentDrawable()
{
    assert(m_objCObj.device);

    if(m_currentDrawable == nullptr ||
       ![m_currentDrawable->objCObj() isEqual:m_objCObj.currentDrawable])
    {
        destroy(m_device->allocator(), m_currentDrawable);
        m_currentDrawable = nullptr;

        id<MTLDrawable> objCDrawable = m_objCObj.currentDrawable;

        if(objCDrawable)
        {
            m_currentDrawable = construct<Drawable>(m_device->allocator(),
                                                    objCDrawable, *m_device);
        }
    }

    return m_currentDrawable;
}


MTL::RenderPassDescriptor *View::currentRenderPassDescriptor()
{
    bool resetDrawable = false;

    if(m_currentRenderPassDescriptor == nullptr)
    {
        m_currentRenderPassDescriptor = construct<RenderPassDescriptor>(m_device->allocator(),
                                                                        m_objCObj.currentRenderPassDescriptor);

        resetDrawable = true;
    }
    else if ( ![m_currentRenderPassDescriptor->objCObj() isEqual:m_objCObj.currentRenderPassDescriptor] )
    {
        resetDrawable = true;
    }

    if(resetDrawable)
    {
        MTL::Texture *drawableTexture = currentDrawable()->texture();

        m_currentRenderPassDescriptor->colorAttachments[0].texture( *drawableTexture );
    }

    if(m_validatedDrawableSize.width != m_objCObj.drawableSize.width &&
       m_validatedDrawableSize.height != m_objCObj.drawableSize.height)
    {
        m_validatedDrawableSize.width = m_objCObj.drawableSize.width;
        m_validatedDrawableSize.height = m_objCObj.drawableSize.height;

        if( depthStencilTexture() )
        {
            m_currentRenderPassDescriptor->depthAttachment.texture( *m_depthStencilTexture );

            m_currentRenderPassDescriptor->stencilAttachment.texture( *m_depthStencilTexture );
        }
    }

    return m_currentRenderPassDescriptor;
}
