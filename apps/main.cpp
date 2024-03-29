//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

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

#include "atomic_compute.h"
#include "cluster_forward_app.h"
#include "multi_light_app.h"
#include "particle_app.h"

int main(int argc, char *argv[]) {
    vox::UnixEngine engine{vox::UnixType::Mac, argc, argv};
    
    auto code = engine.initialize();
    if (code == vox::ExitCode::Success) {
        engine.setApp(std::make_unique<vox::ParticleApp>());
        code = engine.mainLoop();
    }
    
    engine.terminate(code);
    
    return EXIT_SUCCESS;
}
