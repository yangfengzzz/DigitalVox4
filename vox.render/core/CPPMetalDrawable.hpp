/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for C++ Metal drawable class wrapper
*/

#ifndef CPPMetalDrawable_hpp
#define CPPMetalDrawable_hpp

#include "CPPMetalImplementation.hpp"
#include "CPPMetalPixelFormat.hpp"
#include "CPPMetalTypes.hpp"
#include "CPPMetalTexture.hpp"


namespace MTL
{


class Drawable
{
public:

    Drawable() = delete;

    Drawable(const Drawable & rhs);

    Drawable(Drawable && rhs);

    Drawable & operator=(const Drawable & rhs);

    Drawable & operator=(Drawable && rhs);

    CPP_METAL_VIRTUAL ~Drawable();

    Texture *texture();

    void present();

private:

    CPPMetalInternal::Drawable m_objCObj;

    Device *m_device;

    MTL::Texture *m_texture;

public: // Public methods for CPPMetal internal implementation

    Drawable(CPPMetalInternal::Drawable objCObj, Device & device);

    CPPMetalInternal::Drawable objCObj() const;

    void invalidate();
};


//==================================================
#pragma mark - Library inline method implementations

inline Drawable::Drawable(Drawable && rhs)
: m_objCObj(rhs.m_objCObj)
, m_device(rhs.m_device)
, m_texture(rhs.m_texture)
{
    rhs.m_objCObj = nullptr;
}

inline Drawable & Drawable::operator=(Drawable && rhs)
{
    m_objCObj = rhs.m_objCObj;
    m_device = rhs.m_device;
    m_texture = rhs.m_texture;
    rhs.m_objCObj = nullptr;
    rhs.m_device = nullptr;
    rhs.m_texture = nullptr;
    return *this;
}

inline CPPMetalInternal::Drawable Drawable::objCObj() const
{
    return m_objCObj;
}


} // namespace MTL

#endif // CPPMetalDrawable_hpp
