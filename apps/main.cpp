//
//  main.cpp
//  apps
//
//  Created by 杨丰 on 2022/1/13.
//

#include "unix_engine.h"
#include "engine.h"
#include "basic_deferred_app.h"
#include "framebuffer_picker_app.h"
#include "basic_app.h"
#include "primitive_app.h"
#include "physx_app.h"
#include "physx_dynamic_app.h"
#include "animation_app.h"
#include "pbr_app.h"
#include "Irradiance_app.h"
#include "skybox_app.h"

int main(int argc, char * argv[]) {
    vox::UnixEngine engine{vox::UnixType::Mac, argc, argv};
        
    auto code = engine.initialize();
    if (code == vox::ExitCode::Success) {
        engine.setApp(std::make_unique<vox::SkyboxApp>());
        code = engine.mainLoop();
    }
    
    engine.terminate(code);
    
    return EXIT_SUCCESS;
}
