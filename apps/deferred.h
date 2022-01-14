//
//  deferred.hpp
//  apps
//
//  Created by 杨丰 on 2022/1/14.
//

#ifndef deferred_hpp
#define deferred_hpp

#include "metal_application.h"

namespace vox {
class Deferred: public MetalApplication {
public:
    Deferred() = default;
    
    virtual ~Deferred();
    
    /**
     * @brief Additional sample initialization
     */
    virtual bool prepare(Engine &engine) override;
    
    /**
     * @brief Main loop sample events
     */
    virtual void update(float delta_time) override;
};

}

#endif /* deferred_hpp */
