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
MetalApplication::~MetalApplication() {
    render_context.reset();
    device.reset();
    render_pipeline.reset();
}

bool MetalApplication::prepare(Engine &engine) {
    if (!Application::prepare(engine)) {
        return false;
    }
    
    LOG(INFO) << "Initializing Metal Application";
    
    device = std::unique_ptr<MTL::Device>(MTL::CreateSystemDefaultDevice());
    render_context = engine.create_render_context(*device);
    
    render_pipeline = std::make_unique<LightingSubpass>(render_context.get());
    auto extent = engine.get_window().get_extent();
    framebuffer_resize(extent.width*2, extent.height*2);

    return true;
}

void MetalApplication::update(float delta_time) {
    render_context->draw();
    render_pipeline->drawInView(render_context.get());
}

bool MetalApplication::resize(const uint32_t width, const uint32_t height) {
    Application::resize(width, height);
    
    return true;
}

void MetalApplication::framebuffer_resize(uint32_t width, uint32_t height) {
    Application::framebuffer_resize(width, height);

    render_context->resize(MTL::SizeMake(width, height, 0));
    render_pipeline->drawableSizeWillChange(render_context.get(), MTL::SizeMake(width, height, 0));
}

void MetalApplication::input_event(const InputEvent &input_event) {}

void MetalApplication::finish() {}


}
