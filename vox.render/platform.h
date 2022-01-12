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

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>

#include "application.h"
#include "filesystem.h"
#include "window.h"

#if defined(VK_USE_PLATFORM_XLIB_KHR)
#	undef Success
#endif

namespace vox {
enum class ExitCode {
    Success = 0, /* App executed as expected */
    Help,        /* App should show help */
    Close,       /* App has been requested to close at initialization */
    FatalError   /* App encountered an unexpected error */
};

class Platform {
public:
    Platform() = default;
    
    virtual ~Platform() = default;
    
    /**
     * @brief Initialize the platform
     * @return An exit code representing the outcome of initialization
     */
    virtual ExitCode initialize();
    
    /**
     * @brief Handles the main loop of the platform
     * This should be overriden if a platform requires a specific main loop setup.
     * @return An exit code representing the outcome of the loop
     */
    ExitCode main_loop();
    
    /**
     * @brief Runs the application for one frame
     */
    void update();
    
    /**
     * @brief Terminates the platform and the application
     * @param code Determines how the platform should exit
     */
    virtual void terminate(ExitCode code);
    
    /**
     * @brief Requests to close the platform at the next available point
     */
    virtual void close();
    
    virtual void resize(uint32_t width, uint32_t height);
    
    virtual void input_event(const InputEvent &input_event);
    
public:
    void set_focus(bool focused);
    
    void force_simulation_fps(float fps);
    
    void disable_input_processing();
    
public:
    Window &get_window() const;
    
    Application &get_app() const;
    
    Application &get_app();
    
    void set_app(std::unique_ptr<Application>&& active_app);
    
    bool start_app();
    
public:
    /**
     * @brief Returns the working directory of the application set by the platform
     * @returns The path to the working directory
     */
    static const std::string &get_external_storage_directory();
    
    static void set_external_storage_directory(const std::string &dir);
    
    /**
     * @brief Returns the suitable directory for temporary files from the environment variables set in the system
     * @returns The path to the temp folder on the system
     */
    static const std::string &get_temp_directory();
    
    static void set_temp_directory(const std::string &dir);
    
    std::vector<std::string> &get_arguments();
    
    static void set_arguments(const std::vector<std::string> &args);
    
    void set_window_properties(const Window::OptionalProperties &properties);
    
//    void on_post_draw(RenderContext &context) const;
    
    static const uint32_t MIN_WINDOW_WIDTH;
    static const uint32_t MIN_WINDOW_HEIGHT;
    
protected:
    std::unique_ptr<Window> window{nullptr};
    
    std::unique_ptr<Application> active_app{nullptr};
        
    /**
     * @brief Handles the creation of the window
     *
     * @param properties Preferred window configuration
     */
    virtual void create_window(const Window::Properties &properties) = 0;
    
    void on_update(float delta_time);
    
    void on_app_error(const std::string &app_id);
    
    void on_app_start(const std::string &app_id);
    
    void on_app_close(const std::string &app_id);
    
    void on_platform_close();
    
    Window::Properties window_properties;              /* Source of truth for window state */
    bool fixed_simulation_fps{false};    /* Delta time should be fixed with a fabricated value */
    float simulation_frame_time = 0.016f; /* A fabricated delta time */
    bool process_input_events{true};     /* App should continue processing input events */
    bool focused;                        /* App is currently in focus at an operating system level */
    
private:
    Timer timer;
            
    /// Static so can be set via JNI code in android_platform.cpp
    static std::vector<std::string> arguments;
    
    static std::string external_storage_directory;
    
    static std::string temp_directory;
};

}        // namespace vox
