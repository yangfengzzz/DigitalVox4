//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.
#ifndef cpp_mtl_View_hpp
#define cpp_mtl_View_hpp

#include "cpp_mtl_implementation.h"
#include "cpp_mtl_drawable.h"
#include "cpp_mtl_device.h"
#include "cpp_mtl_pixel_format.h"
#include "cpp_mtl_render_pass.h"

namespace vox {
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
    cpp_mtl_internal::MetalLayer m_objCObj;
    cpp_mtl_internal::Drawable m_drawableCObj;

    MTL::Device *m_device;
    
    MTL::Drawable *m_currentDrawable;
    MTL::Texture *m_depthStencilTexture;
    MTL::PixelFormat m_depthStencilPixelFormat;
    MTL::RenderPassDescriptor *m_currentRenderPassDescriptor;
        
public: // Public methods for cpp_mtl_ internal implementation
    cpp_mtl_internal::MetalLayer objCObj() const;
};

//===============================================
#pragma mark - View inline method implementations

inline cpp_mtl_internal::MetalLayer View::objCObj() const {
    return m_objCObj;
}

}

#endif /* cpp_mtl_View_hpp */
