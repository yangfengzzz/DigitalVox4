//
//  CPPMetalView.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/13.
//

#include "metal_view.h"

namespace vox {
using namespace MTL;

View::View(MTL::Device &device) :
m_objCObj([CAMetalLayer layer]),
m_device(&device),
m_currentDrawable(nullptr),
m_currentRenderPassDescriptor(nullptr),
m_depthStencilTexture(nullptr) {
    m_objCObj.device = device.objCObj();
}

View::View(const View &rhs) :
m_objCObj(rhs.m_objCObj),
m_device(rhs.m_device),
m_currentDrawable(nullptr),
m_currentRenderPassDescriptor(nullptr),
m_depthStencilTexture(nullptr) {
    if (rhs.m_depthStencilTexture) {
        m_depthStencilTexture = construct<Texture>(m_device->allocator(), *rhs.m_depthStencilTexture);
    }
}

View::~View() {
    destroy(m_device->allocator(), m_currentDrawable);
    destroy(m_device->allocator(), m_currentRenderPassDescriptor);
    destroy(m_device->allocator(), m_depthStencilTexture);
}

void View::draw() {
    m_drawableCObj = [m_objCObj nextDrawable];
}

MTL::Drawable *View::currentDrawable() {
    assert(m_objCObj.device);
    
    if(m_currentDrawable == nullptr ||
       ![m_currentDrawable->objCObj() isEqual:m_drawableCObj]) {
        destroy(m_device->allocator(), m_currentDrawable);
        m_currentDrawable = nullptr;
        
        id<MTLDrawable> objCDrawable = m_drawableCObj;
        
        if(objCDrawable) {
            m_currentDrawable = construct<Drawable>(m_device->allocator(),
                                                    objCDrawable, *m_device);
        }
    }
    
    return m_currentDrawable;
}

Texture *View::depthStencilTexture() {
    if ((!m_depthStencilTexture) ||
        (m_depthStencilTexture->width() != m_objCObj.drawableSize.width ||
         m_depthStencilTexture->height() != m_objCObj.drawableSize.height)) {
        destroy(m_device->allocator(), m_depthStencilTexture);
        m_depthStencilTexture = nullptr;
        
        MTLTextureDescriptor *descriptor =
        [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:(MTLPixelFormat) m_depthStencilPixelFormat
                                                           width:m_objCObj.drawableSize.width
                                                          height:m_objCObj.drawableSize.height
                                                       mipmapped:false];
        descriptor.usage = MTLTextureUsageShaderRead | MTLTextureUsageRenderTarget;
        descriptor.storageMode = MTLStorageModePrivate;
        
        m_depthStencilTexture = construct<Texture>(m_device->allocator(),
                                                   [m_device->objCObj() newTextureWithDescriptor:descriptor],
                                                   *m_device);
    }
    return m_depthStencilTexture;
}

MTL::RenderPassDescriptor *View::currentRenderPassDescriptor() {
    if (m_currentRenderPassDescriptor == nullptr) {
        MTLRenderPassDescriptor *renderPassDescriptor = [[MTLRenderPassDescriptor alloc] init];
        m_currentRenderPassDescriptor = construct<RenderPassDescriptor>(m_device->allocator(),
                                                                        renderPassDescriptor);
    }
    m_currentRenderPassDescriptor->colorAttachments[0].texture(*currentDrawable()->texture());
    
    if (m_depthStencilTexture->width() != m_objCObj.drawableSize.width ||
        m_depthStencilTexture->height() != m_objCObj.drawableSize.height) {
        
        destroy(m_device->allocator(), m_depthStencilTexture);
        m_depthStencilTexture = nullptr;
        if (depthStencilTexture()) {
            m_currentRenderPassDescriptor->depthAttachment.texture(*m_depthStencilTexture);
            m_currentRenderPassDescriptor->stencilAttachment.texture(*m_depthStencilTexture);
        }
    }
    
    return m_currentRenderPassDescriptor;
}

MTL::Size View::drawableSize() const {
    MTL::Size size = SizeMake(m_objCObj.drawableSize.width, m_objCObj.drawableSize.height, 0);
    return size;
}

void View::resize(const MTL::Size &size) {
    m_objCObj.drawableSize = CGSizeMake(size.width, size.height);
}

MTL::PixelFormat View::depthStencilPixelFormat() const {
    return m_depthStencilPixelFormat;
}

void View::depthStencilPixelFormat(MTL::PixelFormat format) {
    m_depthStencilPixelFormat = format;
}

MTL::PixelFormat View::colorPixelFormat() const {
    return (MTL::PixelFormat) m_objCObj.pixelFormat;
}

void View::colorPixelFormat(MTL::PixelFormat format) {
    m_objCObj.pixelFormat = (MTLPixelFormat) format;
}

Device &View::device() {
    return *m_device;
}

}
