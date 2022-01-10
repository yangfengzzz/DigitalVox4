/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Implementation of C++ Metal crawable class wrapper
*/

#include "CPPMetalDrawable.hpp"
#include "CPPMetalDevice.hpp"
#include <Metal/Metal.h>


using namespace MTL;


Drawable::Drawable(CPPMetalInternal::Drawable objCObj,
                   Device & device)
: m_objCObj(objCObj)
, m_device(&device)
, m_texture(nullptr)
{
    // Member initialization only
}

Drawable::Drawable(const Drawable & rhs)
: m_objCObj(rhs.m_objCObj)
, m_device(rhs.m_device)
, m_texture(nullptr)
{
}

Drawable & Drawable::operator=(const Drawable & rhs)
{
    m_objCObj = rhs.m_objCObj;
    m_device = rhs.m_device;
    m_texture = nullptr;
    return *this;
}

Drawable::~Drawable()
{
    m_objCObj = nil;
    destroy(m_device->allocator(), m_texture);
}

void Drawable::invalidate()
{
    destroy(m_device->allocator(), m_texture);
    m_texture = nullptr;
    m_objCObj = nil;
}

Texture *Drawable::texture()
{
    // Drawable::present() or CommandBuffer::presentDrawable() already called on this drawable.
    // Caller should not have called this function because it's too late to get the texture.
    assert(m_objCObj);

    if(!m_texture)
    {
        m_texture = construct<Texture>(m_device->allocator(),
                                       ((id<CAMetalDrawable>)m_objCObj).texture,
                                       *m_device);
    }

    return m_texture;
}

void Drawable::present()
{
    // Drawable::present() or CommentBuffer::presentDrawable() already called on this drawable.
    // Caller should not have called because clients can only present a drawable once
    assert(m_objCObj);

    [m_objCObj present];

    invalidate();
}
