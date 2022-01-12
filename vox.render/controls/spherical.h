//
//  spherical.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/30.
//

#ifndef spherical_hpp
#define spherical_hpp

#include "ImathVec.h"

namespace vox {
namespace control {
// Spherical.
class Spherical {
public:
    Spherical(float radius = 1.0, float phi = 0, float theta = 0);
    
    void set(float radius, float phi, float theta);
    
    void makeSafe();
    
    void setFromVec3(const Imath::V3f &v3);
    
    void setToVec3(Imath::V3f &v3);
    
private:
    friend class OrbitControl;
    
    friend class FreeControl;
    
    float radius;
    float phi;
    float theta;
};

}
}

#endif /* spherical_hpp */
