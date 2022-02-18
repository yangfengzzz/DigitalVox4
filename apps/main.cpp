//
//  main.cpp
//  apps
//
//  Created by 杨丰 on 2022/1/13.
//

#include "unix_engine.h"
#include "engine.h"
#include "framebuffer_picker_app.h"
#include "primitive_app.h"
#include "physx_app.h"
#include "physx_dynamic_app.h"
#include "animation_app.h"
#include "pbr_app.h"
#include "Irradiance_app.h"
#include "skybox_app.h"
#include "ibl_app.h"
#include "gltf_app.h"
#include "scene_animation_app.h"
#include "shadowmap_app.h"
#include "cascade_shadowmap_app.h"
#include "omni_shadowmap_app.h"

#include "cloth/cloth_apps.h"

int main(int argc, char * argv[]) {
    vox::UnixEngine engine{vox::UnixType::Mac, argc, argv};
        
    auto code = engine.initialize();
    if (code == vox::ExitCode::Success) {
        engine.setApp(std::make_unique<vox::cloth::LocalGlobalApp>());
        code = engine.mainLoop();
    }
    
    engine.terminate(code);
    
    return EXIT_SUCCESS;
}
