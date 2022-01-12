//
//  updateFlag.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/11/26.
//

#ifndef updateFlag_hpp
#define updateFlag_hpp

#include <stdio.h>

namespace vox {
namespace sg {
class UpdateFlagManager;

/**
 * Used to update tags.
 */
class UpdateFlag {
public:
    bool flag = true;
    
    UpdateFlag(UpdateFlagManager *_flags);
    
    /**
     * Destroy.
     */
    void destroy();
    
private:
    UpdateFlagManager *_flags;
};

}        // namespace sg
}        // namespace vox

#endif /* updateFlag_hpp */
