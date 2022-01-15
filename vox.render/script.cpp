//
//  script.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/12.
//

#include "script.h"
#include "entity.h"
#include "scene.h"

namespace vox {
Script::Script(Entity *entity) :
Component(entity) {
    
}

void Script::_onAwake() {
    onAwake();
}

void Script::_onEnable() {
    auto &componentsManager = entity()->scene()->_componentsManager;
    if (!_started) {
        componentsManager.addOnStartScript(this);
    }
    //todo fix
    componentsManager.addOnUpdateScript(this);
    componentsManager.addOnLateUpdateScript(this);
    componentsManager.addOnEndFrameScript(this);
    _entity->_addScript(this);
    onEnable();
}

void Script::_onDisable() {
    auto &componentsManager = entity()->scene()->_componentsManager;
    // Use "xxIndex" is more safe.
    // When call onDisable it maybe it still not in script queue,for example write "entity.isActive = false" in onWake().
    if (_onStartIndex != -1) {
        componentsManager.removeOnStartScript(this);
    }
    if (_onUpdateIndex != -1) {
        componentsManager.removeOnUpdateScript(this);
    }
    if (_onLateUpdateIndex != -1) {
        componentsManager.removeOnLateUpdateScript(this);
    }
    if (_onEndFrameIndex != -1) {
        componentsManager.removeOnEndFrameScript(this);
    }
    if (_entityCacheIndex != -1) {
        _entity->_removeScript(this);
    }
    onDisable();
}

void Script::_onDestroy() {
    entity()->scene()->_componentsManager.addDestroyComponent(this);
}

}
