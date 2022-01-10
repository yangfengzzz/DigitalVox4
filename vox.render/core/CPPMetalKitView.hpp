/*
See LICENSE folder for this sample’s licensing information.

Abstract:
Header for C++ MetalKit view wrapper
*/

#ifndef CPPMetalKitView_hpp
#define CPPMetalKitView_hpp

#include "CPPMetalImplementation.hpp"
#include "CPPMetalPixelFormat.hpp"
#include "CPPMetalTypes.hpp"


namespace MTL
{

class Drawable;
class RenderPassDescriptor;
class Device;
class Texture;

}


namespace MTK
{


class View
{
public:

    View() = delete;

    View(const View & rhs);

    View & operator=(const View & rhs) = delete;

    View(CPPMetalInternal::View objCObj, MTL::Device & device);

    CPP_METAL_VIRTUAL ~View();

    void draw();

    MTL::Drawable *currentDrawable();

    MTL::RenderPassDescriptor *currentRenderPassDescriptor();

    MTL::Texture *depthStencilTexture();

    MTL::Size drawableSize() const;

    bool isPaused() const;
    void paused(bool pause);

    bool isHidden() const;
    void hidden(bool hidden);

    MTL::PixelFormat depthStencilPixelFormat() const;
    void             depthStencilPixelFormat(MTL::PixelFormat format);

    MTL::PixelFormat colorPixelFormat() const;
    void             colorPixelFormat(MTL::PixelFormat format);

    MTL::Device & device();

private:

    CPPMetalInternal::View m_objCObj;

    MTL::Device *m_device;

    MTL::Drawable *m_currentDrawable;

    MTL::RenderPassDescriptor *m_currentRenderPassDescriptor;

    MTL::Texture *m_depthStencilTexture;

    MTL::Size m_validatedDrawableSize;

public: // Public methods for CPPMetal internal implementation

    CPPMetalInternal::View objCObj() const;

};


//===============================================
#pragma mark - View inline method implementations

inline CPPMetalInternal::View View::objCObj() const
{
    return m_objCObj;
}


} // namespace MTK

#endif // CPPMetalKitView_hpp
