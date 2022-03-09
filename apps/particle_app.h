//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef particle_app_hpp
#define particle_app_hpp

#include "forward_application.h"

namespace vox {
class ParticleApp : public ForwardApplication {
public:
    bool prepare(Engine &engine) override;
    
    void loadScene() override;
};

}
#endif /* particle_app_hpp */
