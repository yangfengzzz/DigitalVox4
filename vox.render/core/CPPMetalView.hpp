//
//  CPPMetalView.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/13.
//

#ifndef CPPMetalView_hpp
#define CPPMetalView_hpp

#include "CPPCAMetalLayer.hpp"
#include "CPPMetalRenderPass.hpp"

namespace MTL {
class View {
public:
    View() = delete;

    View(const View & rhs);

    View & operator=(const View & rhs) = delete;

    View(CA::MetalLayer & layer, MTL::Device & device);

    CPP_METAL_VIRTUAL ~View();

    void draw();

    MTL::Drawable *currentDrawable();

    MTL::RenderPassDescriptor *currentRenderPassDescriptor();

    MTL::Texture *depthStencilTexture();

    MTL::Size drawableSize() const;
    
    void resize(const MTL::Size& size);

    MTL::PixelFormat depthStencilPixelFormat() const;
    void             depthStencilPixelFormat(MTL::PixelFormat format);

    MTL::PixelFormat colorPixelFormat() const;
    void             colorPixelFormat(MTL::PixelFormat format);

    MTL::Device & device();

private:
    CA::MetalLayer m_layer;

    MTL::Device *m_device;

    MTL::Drawable *m_currentDrawable;

    MTL::RenderPassDescriptor *m_currentRenderPassDescriptor;

    MTL::Texture *m_depthStencilTexture;

    MTL::Size m_validatedDrawableSize;
};


}

#endif /* CPPMetalView_hpp */
