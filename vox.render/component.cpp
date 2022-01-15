/* Copyright (c) 2018-2019, Arm Limited and Contributors
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

#include "component.h"

#include <algorithm>

#include "entity.h"

namespace vox {
Component::Component(Entity *entity) : _entity(entity) {
}

bool Component::enabled() {
    return _enabled;
}

void Component::setEnabled(bool value) {
    if (value == _enabled) {
        return;
    }
    _enabled = value;
    if (value) {
        if (_entity->isActiveInHierarchy()) {
            _onEnable();
        }
    } else {
        if (_entity->isActiveInHierarchy()) {
            _onDisable();
        }
    }
}

bool Component::destroyed() {
    return _destroyed;
}

Entity *Component::entity() const {
    return _entity;
}

Scene *Component::scene() {
    return _entity->scene();
}

void Component::destroy() {
    if (_destroyed) {
        return;
    }
    _entity->_removeComponent(this);
    if (_entity->isActiveInHierarchy()) {
        if (_enabled) {
            _onDisable();
        }
        _onInActive();
    }
    _destroyed = true;
    _onDestroy();
}

void Component::_setActive(bool value) {
    if (value) {
        if (!_awoken) {
            _awoken = true;
            _onAwake();
        }
        // You can do isActive = false in onAwake function.
        if (_entity->_isActiveInHierarchy) {
            _onActive();
            if (_enabled) {
                _onEnable();
            }
        }
    } else {
        if (_enabled) {
            _onDisable();
        }
        _onInActive();
    }
}

}        // namespace vox
