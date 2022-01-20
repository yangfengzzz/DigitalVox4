//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "metal_application.h"
#include "engine.h"
#include "shader/shader.h"

#include "core/cpp_mtl_assert.h"
#include <glog/logging.h>

namespace vox {
MetalApplication::MetalApplication() {
    Shader::create("unlit", "vertex_unlit", "fragment_unlit");
    Shader::create("blinn-phong", "vertex_blinn_phong", "fragment_blinn_phong");
    Shader::create("particle-shader", "vertex_particle", "fragment_particle");
    Shader::create("pbr", "vertex_blinn_phong", "fragment_pbr");
    Shader::create("pbr-specular", "vertex_blinn_phong", "fragment_pbr");
    Shader::create("skybox", "vertex_skybox", "fragment_skybox");
    
    // MARK: - experimental
    Shader::create("shadow-map", "vertex_shadow_map", "fragment_shadow_map");
    Shader::create("shadow", "vertex_shadow_map", "fragment_shadow");
    Shader::create("background-texture", "vertex_background_texture", "fragment_background_texture");
    
    Shader::create("experimental", "vertex_experimental", "fragment_experimental");
}

MetalApplication::~MetalApplication() {
    _renderView.reset();
    _device.reset();
}

bool MetalApplication::prepare(Engine &engine) {
    if (!Application::prepare(engine)) {
        return false;
    }
    
    LOG(INFO) << "Initializing Metal Application";
    
    _device = std::unique_ptr<MTL::Device>(MTL::CreateSystemDefaultDevice());
    printf("Selected Device: %s\n", _device->name());
    
    _library = makeShaderLibrary();
    
    _commandQueue = _device->makeCommandQueue();

    auto extent = engine.window().extent();

    _renderView = engine.createRenderView(*_device);
    _renderView->resize(MTL::sizeMake(extent.width * 2, extent.height * 2, 0));
    _renderView->depthStencilPixelFormat(MTL::PixelFormatDepth32Float_Stencil8);
    _renderView->colorPixelFormat(MTL::PixelFormatBGRA8Unorm_sRGB);    
    return true;
}

void MetalApplication::update(float delta_time) {
    _renderView->draw();
}

bool MetalApplication::resize(const uint32_t width, const uint32_t height) {
    Application::resize(width, height);
    _renderView->resize(MTL::sizeMake(width, height, 0));
    return true;
}

void MetalApplication::inputEvent(const InputEvent &inputEvent) {}

void MetalApplication::finish() {}

MTL::Library MetalApplication::makeShaderLibrary() {
    CFErrorRef error = nullptr;
    CFURLRef libraryURL = nullptr;

    libraryURL = CFBundleCopyResourceURL( CFBundleGetMainBundle() , CFSTR("vox.shader"), CFSTR("metallib"), nullptr);
    MTL::Library shaderLibrary = _device->makeLibrary(libraryURL, &error);
    
    MTLAssert(!error, error, "Could not load Metal shader library");
    
    CFRelease(libraryURL);
    return shaderLibrary;
}


}
