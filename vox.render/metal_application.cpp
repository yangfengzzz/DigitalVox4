//
//  metal_application.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/12.
//

#include "metal_application.h"
#include "engine.h"
#include "rendering/lighting_subpass.h"
#include <glog/logging.h>

namespace vox {
bool MetalApplication::prepare(Engine &engine) {
    if (!Application::prepare(engine)) {
        return false;
    }
    
    LOG(INFO) << "Initializing Metal Application";
    
    device = std::unique_ptr<MTL::Device>(MTL::CreateSystemDefaultDevice());
    render_context = engine.create_render_context(*device);
    
    render_pipeline = std::make_unique<LightingSubpass>(*render_context);
    
    return true;
}

void MetalApplication::update(float delta_time) {
    render_pipeline->drawInView(*render_context);
}

bool MetalApplication::resize(const uint32_t width, const uint32_t height) {
    Application::resize(width, height);
    
    return true;
}

void MetalApplication::framebuffer_resize(uint32_t width, uint32_t height) {
    Application::framebuffer_resize(width, height);

    render_context->resize(MTL::SizeMake(width, height, 0));
    render_pipeline->drawableSizeWillChange(*render_context, MTL::SizeMake(width, height, 0));
}



}
