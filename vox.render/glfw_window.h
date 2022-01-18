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

#include "window.h"

struct GLFWwindow;

namespace vox {
class Engine;

/**
 * @brief An implementation of GLFW, inheriting the behaviour of the Window interface
 */
class GlfwWindow : public Window {
public:
    GlfwWindow(Engine *engine, const Window::Properties &properties);
    
    virtual ~GlfwWindow();
    
    void setView(const View& view) override;
    
    bool shouldClose() override;
    
    void processEvents() override;
    
    void close() override;
    
    float dpiFactor() const override;
    
    float contentScaleFactor() const override;
    
private:
    GLFWwindow *_handle = nullptr;
};
}        // namespace vox
