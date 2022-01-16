/* Copyright (c) 2019-2021, Arm Limited and Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 the "License";
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "engine.h"

#include <algorithm>
#include <ctime>
#include <mutex>
#include <vector>
#include <glog/logging.h>
#include <iostream>

#include "filesystem.h"

namespace vox {
const uint32_t Engine::MIN_WINDOW_WIDTH = 420;
const uint32_t Engine::MIN_WINDOW_HEIGHT = 320;

std::vector<std::string> Engine::arguments = {};

std::string Engine::external_storage_directory = "";

std::string Engine::temp_directory = "";

ExitCode Engine::initialize() {
    google::InitGoogleLogging("DigitalVox");
    
    create_window(window_properties);
    if (!window) {
        LOG(ERROR) << "Window creation failed!";
        return ExitCode::FatalError;
    }
    
    return ExitCode::Success;
}

ExitCode Engine::main_loop() {
    // Load the requested app
    if (!start_app()) {
        LOG(ERROR) << "Failed to load requested application";
        return ExitCode::FatalError;
    }
    
    // Compensate for load times of the app by rendering the first frame pre-emptively
    timer.tick<Timer::Seconds>();
    active_app->update(0.01667f);
    
    while (!window->should_close()) {
        try {
            update();
            
            window->process_events();
        }
        catch (std::exception &e) {
            LOG(ERROR) << "Error Message: " << e.what();
            LOG(ERROR) << "Failed when running application " << active_app->get_name();
            
            return ExitCode::FatalError;
        }
    }
    
    return ExitCode::Success;
}

void Engine::update() {
    auto delta_time = static_cast<float>(timer.tick<Timer::Seconds>());
    
    if (focused) {
        if (fixed_simulation_fps) {
            delta_time = simulation_frame_time;
        }
        
        active_app->update(delta_time);
    }
}

void Engine::terminate(ExitCode code) {
    if (active_app) {
        active_app->finish();
    }
    
    active_app.reset();
    window.reset();
        
    // Halt on all unsuccessful exit codes unless ForceClose is in use
    if (code != ExitCode::Success) {
        std::cout << "Press any key to continue";
        std::cin.get();
    }
}

void Engine::close() {
    if (window) {
        window->close();
    }
}

void Engine::input_event(const InputEvent &input_event) {
    if (process_input_events && active_app) {
        active_app->input_event(input_event);
    }
    
    if (input_event.get_source() == EventSource::Keyboard) {
        const auto &key_event = static_cast<const KeyInputEvent &>(input_event);
        
        if (key_event.get_code() == KeyCode::Back ||
            key_event.get_code() == KeyCode::Escape) {
            close();
        }
    }
}

void Engine::resize(uint32_t width, uint32_t height) {
    auto extent = Window::Extent{std::max<uint32_t>(width, MIN_WINDOW_WIDTH), std::max<uint32_t>(height, MIN_WINDOW_HEIGHT)};
    if (window) {
        auto actual_extent = window->resize(extent);
        
        if (active_app) {
            active_app->resize(actual_extent.width, actual_extent.height);
        }
    }
}

void Engine::framebuffer_resize(uint32_t width, uint32_t height) {
    if (active_app) {
        active_app->framebuffer_resize(width, height);
    }
}

std::unique_ptr<View> Engine::create_render_context(MTL::Device &device) {
    auto context = std::make_unique<View>(device);
    window->set_view(*context);
    return context;
}

//MARK: - Application
Window &Engine::get_window() const {
    return *window;
}

Application &Engine::get_app() {
    assert(active_app && "Application is not valid");
    return *active_app;
}

Application &Engine::get_app() const {
    assert(active_app && "Application is not valid");
    return *active_app;
}

void Engine::set_app(std::unique_ptr<Application>&& new_app) {
    if (active_app) {
        auto execution_time = timer.stop();
        LOG(INFO) << "Closing App (Runtime: " << execution_time << ")";
        
        auto app_id = active_app->get_name();
        
        active_app->finish();
    }
    
    active_app = std::move(new_app);
}

bool Engine::start_app() {
    active_app->set_name("");
    
    if (!active_app) {
        LOG(ERROR) << "Failed to create a valid vulkan app.";
        return false;
    }
    
    if (!active_app->prepare(*this)) {
        LOG(ERROR) << "Failed to prepare vulkan app.";
        return false;
    }
        
    return true;
}

//MARK: - Controller
void Engine::force_simulation_fps(float fps) {
    fixed_simulation_fps = true;
    simulation_frame_time = 1 / fps;
}

void Engine::disable_input_processing() {
    process_input_events = false;
}

void Engine::set_focus(bool _focused) {
    focused = _focused;
}

//MARK: - Property
const std::string &Engine::get_external_storage_directory() {
    return external_storage_directory;
}

void Engine::set_external_storage_directory(const std::string &dir) {
    external_storage_directory = dir;
}

const std::string &Engine::get_temp_directory() {
    return temp_directory;
}

void Engine::set_temp_directory(const std::string &dir) {
    temp_directory = dir;
}

std::vector<std::string> &Engine::get_arguments() {
    return Engine::arguments;
}

void Engine::set_arguments(const std::vector<std::string> &args) {
    arguments = args;
}

void Engine::set_window_properties(const Window::OptionalProperties &properties) {
    window_properties.title = properties.title.has_value() ? properties.title.value() : window_properties.title;
    window_properties.mode = properties.mode.has_value() ? properties.mode.value() : window_properties.mode;
    window_properties.resizable = properties.resizable.has_value() ? properties.resizable.value() : window_properties.resizable;
    window_properties.vsync = properties.vsync.has_value() ? properties.vsync.value() : window_properties.vsync;
    window_properties.extent.width = properties.extent.width.has_value() ? properties.extent.width.value() : window_properties.extent.width;
    window_properties.extent.height = properties.extent.height.has_value() ? properties.extent.height.value() : window_properties.extent.height;
}





}        // namespace vox
