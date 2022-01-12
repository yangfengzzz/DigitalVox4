//
//  CPPCAMetalLayer.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/12.
//

#ifndef CPPCAMetalLayer_hpp
#define CPPCAMetalLayer_hpp

#include "CPPMetalImplementation.hpp"
#include "CPPMetalDrawable.hpp"
#include "CPPMetalDevice.hpp"
#include "CPPMetalPixelFormat.hpp"

namespace CA {

class MetalLayer {
public:
    MetalLayer();
    
    MTL::Drawable nextDrawable();
    
    void setDevice(MTL::Device &device);
    
    void pixelFormat(MTL::PixelFormat format);
    
    MTL::PixelFormat pixelFormat() const;
    
private:
    CPPMetalInternal::MetalLayer m_objCObj;
    MTL::Device *m_device;
    
public:
    CPPMetalInternal::MetalLayer objCObj() const;

};

}

#endif /* CPPCAMetalLayer_hpp */
