//
//  main.cpp
//  apps
//
//  Created by 杨丰 on 2022/1/13.
//

#include "unix_engine.h"
#include "engine.h"
#include "basic_deferred_app.h"
#include "basic_app.h"
#include "primitive_app.h"
#include "physx_app.h"

int main(int argc, char * argv[]) {
    vox::UnixEngine engine{vox::UnixType::Mac, argc, argv};
        
    auto code = engine.initialize();
    if (code == vox::ExitCode::Success) {
//        engine.setApp(std::make_unique<vox::BasicDeferredApp>());
        engine.setApp(std::make_unique<vox::PhysXApp>());
        code = engine.mainLoop();
    }
    
    engine.terminate(code);
    
    return EXIT_SUCCESS;
}
