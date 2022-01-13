//
//  script.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/12.
//

#ifndef script_hpp
#define script_hpp

#include "component.h"
#include "input_events.h"

namespace vox {
namespace physics {
class ColliderShape;

using ColliderShapePtr = std::shared_ptr<ColliderShape>;
}

namespace sg {
/**
 * Script class, used for logic writing.
 */
class Script : public Component {
public:
    explicit Script(Entity *entity);
    
    /**
     * Called when be enabled first time, only once.
     */
    virtual void onAwake() {
    }
    
    /**
     * Called when be enabled.
     */
    virtual void onEnable() {
    }
    
    /**
     * Called before the frame-level loop start for the first time, only once.
     */
    virtual void onStart() {
    }
    
    /**
     * The main loop, called frame by frame.
     * @param deltaTime - The deltaTime when the script update.
     */
    virtual void onUpdate(float deltaTime) {
    }
    
    /**
     * Called after the onUpdate finished, called frame by frame.
     * @param deltaTime - The deltaTime when the script update.
     */
    virtual void onLateUpdate(float deltaTime) {
    }
    
    /**
     * Called before camera rendering, called per camera.
     * @param camera - Current camera.
     */
    virtual void onBeginRender(Camera *camera) {
    }
    
    /**
     * Called after camera rendering, called per camera.
     * @param camera - Current camera.
     */
    virtual void onEndRender(Camera *camera) {
    }
    
    /**
     * Called after command buffer commit.
     */
    virtual void onEndFrame() {
    }
    
    /**
     * Called when the collision enter.
     * @param other ColliderShape
     */
    virtual void onTriggerEnter(physics::ColliderShapePtr other) {
    }
    
    /**
     * Called when the collision stay.
     * @remarks onTriggerStay is called every frame while the collision stay.
     * @param other ColliderShape
     */
    virtual void onTriggerExit(physics::ColliderShapePtr other) {
    }
    
    /**
     * Called when the collision exit.
     * @param other ColliderShape
     */
    virtual void onTriggerStay(physics::ColliderShapePtr other) {
    }
    
    virtual void inputEvent(const InputEvent &input_event) {
    }
    
    virtual void resize(uint32_t width, uint32_t height) {
    }
    
    /**
     * Called when be disabled.
     */
    virtual void onDisable() {
    }
    
    /**
     * Called at the end of the destroyed frame.
     */
    virtual void onDestroy() {
    }
    
    
private:
    friend class Entity;
    
    friend class ComponentsManager;
    
    void _onAwake() override;
    
    void _onEnable() override;
    
    void _onDisable() override;
    
    void _onDestroy() override;
    
    bool _started = false;
    ssize_t _onStartIndex = -1;
    ssize_t _onUpdateIndex = -1;
    ssize_t _onLateUpdateIndex = -1;
    ssize_t _onPreRenderIndex = -1;
    ssize_t _onPostRenderIndex = -1;
    ssize_t _onEndFrameIndex = -1;
    ssize_t _entityCacheIndex = -1;
};

}
}

#endif /* script_hpp */
