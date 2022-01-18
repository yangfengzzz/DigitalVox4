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

#include "input_events.h"

namespace vox {
InputEvent::InputEvent(EventSource source) :
_source{source} {
}

EventSource InputEvent::source() const {
    return _source;
}

//MARK: - Key Event
KeyInputEvent::KeyInputEvent(KeyCode code, KeyAction action) :
InputEvent{EventSource::Keyboard},
_code{code},
_action{action} {
}

KeyCode KeyInputEvent::code() const {
    return _code;
}

KeyAction KeyInputEvent::action() const {
    return _action;
}

//MARK: - Mouse Event
MouseButtonInputEvent::MouseButtonInputEvent(MouseButton button, MouseAction action, float pos_x, float pos_y) :
InputEvent{EventSource::Mouse},
_button{button},
_action{action},
_pos_x{pos_x},
_pos_y{pos_y} {
}

MouseButton MouseButtonInputEvent::button() const {
    return _button;
}

MouseAction MouseButtonInputEvent::action() const {
    return _action;
}

float MouseButtonInputEvent::pos_x() const {
    return _pos_x;
}

float MouseButtonInputEvent::pos_y() const {
    return _pos_y;
}

//MARK: - Scroll Event
ScrollInputEvent::ScrollInputEvent(float offset_x, float offset_y):
InputEvent{EventSource::Scroll},
_offset_x(offset_x),
_offset_y(offset_y) {
}

float ScrollInputEvent::offset_x() const {
    return _offset_x;
}

float ScrollInputEvent::offset_y() const {
    return _offset_y;
}

//MARK: - Touch Event
TouchInputEvent::TouchInputEvent(int32_t pointer_id, std::size_t touch_points, TouchAction action, float pos_x, float pos_y)
:
InputEvent{EventSource::Touchscreen},
_action{action},
_pointer_id{pointer_id},
_touch_points{touch_points},
_pos_x{pos_x},
_pos_y{pos_y} {
}

TouchAction TouchInputEvent::action() const {
    return _action;
}

int32_t TouchInputEvent::pointer_id() const {
    return _pointer_id;
}

std::size_t TouchInputEvent::touch_points() const {
    return _touch_points;
}

float TouchInputEvent::pos_x() const {
    return _pos_x;
}

float TouchInputEvent::pos_y() const {
    return _pos_y;
}
}        // namespace vox
