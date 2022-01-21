//
//  editor.h
//  editor
//
//  Created by 杨丰 on 2022/1/20.
//

#ifndef editor_hpp
#define editor_hpp

#include "editor_application.h"

namespace vox {
class Editor : public EditorApplication {
public:
    void loadScene(uint32_t width, uint32_t height) override;
    
    void pickFunctor(Renderer *renderer, MeshPtr mesh) override;
    
    void inputEvent(const InputEvent &inputEvent) override;
};
 
}

#endif /* editor_hpp */
