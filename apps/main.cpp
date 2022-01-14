//
//  main.cpp
//  apps
//
//  Created by 杨丰 on 2022/1/13.
//

#include "unix_engine.h"
#include "engine.h"
#include "deferred.h"

int main(int argc, char * argv[]) {
    vox::UnixEngine engine{vox::UnixType::Mac, argc, argv};
        
    auto code = engine.initialize();
    if (code == vox::ExitCode::Success) {
        engine.set_app(std::make_unique<vox::Deferred>());
        code = engine.main_loop();
    }
    
    engine.terminate(code);
    
    return EXIT_SUCCESS;
}
