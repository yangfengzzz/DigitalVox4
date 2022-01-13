//
//  CPPMetalView.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/13.
//

#ifndef CPPMetalView_hpp
#define CPPMetalView_hpp

#include "CPPMetalImplementation.hpp"
#include "CPPMetalDrawable.hpp"
#include "CPPMetalDevice.hpp"
#include "CPPMetalPixelFormat.hpp"
#include "CPPMetalRenderPass.hpp"

namespace MTL {
class View {
public:
    View() = delete;
    
    View(const View &rhs);
    
    View &operator=(const View &rhs) = delete;
    
    View(MTL::Device &device);
    
    CPP_METAL_VIRTUAL ~View();
    
    void draw();
    
    MTL::Drawable *currentDrawable();
    
    MTL::Texture *depthStencilTexture();
    
    MTL::RenderPassDescriptor *currentRenderPassDescriptor();
    
    MTL::Size drawableSize() const;
    
    void resize(const MTL::Size &size);
    
    MTL::PixelFormat depthStencilPixelFormat() const;
    
    void depthStencilPixelFormat(MTL::PixelFormat format);
    
    MTL::PixelFormat colorPixelFormat() const;
    
    void colorPixelFormat(MTL::PixelFormat format);
    
    MTL::Device &device();
    
private:
    CPPMetalInternal::MetalLayer m_objCObj;
    CPPMetalInternal::Drawable m_drawableCObj;

    MTL::Device *m_device;
    
    MTL::Drawable *m_currentDrawable;
    MTL::Texture *m_depthStencilTexture;
    MTL::PixelFormat m_depthStencilPixelFormat;
    MTL::RenderPassDescriptor *m_currentRenderPassDescriptor;
        
public: // Public methods for CPPMetal internal implementation
    CPPMetalInternal::MetalLayer objCObj() const;
};

//===============================================
#pragma mark - View inline method implementations

inline CPPMetalInternal::MetalLayer View::objCObj() const {
    return m_objCObj;
}

}

#endif /* CPPMetalView_hpp */
