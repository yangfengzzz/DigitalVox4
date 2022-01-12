//
//  CPPCAMetalLayer.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/12.
//

#include "CPPCAMetalLayer.hpp"
#include "CPPMetalInternalMacros.h"

namespace CA {
using namespace MTL;

MetalLayer::MetalLayer() {
    m_objCObj = [CAMetalLayer layer];
}

CPPMetalInternal::MetalLayer MetalLayer::objCObj() const {
    return m_objCObj;
}

void MetalLayer::setDevice(MTL::Device &device) {
    m_objCObj.device = device.objCObj();
    m_device = &device;
}

CPP_METAL_READWRITE_MTL_ENUM_PROPERTY_IMPLEMENTATION(MetalLayer, PixelFormat, pixelFormat);

MTL::Drawable MetalLayer::nextDrawable() {
    return MTL::Drawable([m_objCObj nextDrawable], *m_device);
}

}
