//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "application.h"

#include "engine.h"
#include "window.h"

namespace vox {
Application::Application() :
_name{"Sample Name"} {
}

bool Application::prepare(Engine &engine) {
    _engine = &engine;
    return true;
}

void Application::finish() {
}

bool Application::resize(const uint32_t /*width*/, const uint32_t /*height*/) {
    return true;
}

void Application::framebufferResize(uint32_t width, uint32_t height) {
}

void Application::inputEvent(const InputEvent &inputEvent) {
}

void Application::update(float delta_time) {
    _fps = 1.0f / delta_time;
    _frameTime = delta_time * 1000.0f;
}

const std::string &Application::name() const {
    return _name;
}

void Application::setName(const std::string &name) {
    _name = name;
}

}        // namespace vox
