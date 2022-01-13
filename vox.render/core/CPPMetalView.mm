//
//  CPPMetalView.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/13.
//

#include "CPPMetalView.hpp"

namespace MTL {
View::View(CA::MetalLayer & layer, MTL::Device & device) :
m_layer(layer),
m_device(&device),
m_currentDrawable(nullptr),
m_currentRenderPassDescriptor(nullptr),
m_depthStencilTexture(nullptr) {
    m_layer.setDevice(device);
}


}
