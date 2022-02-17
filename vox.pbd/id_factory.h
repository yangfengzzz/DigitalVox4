//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef id_factory_h
#define id_factory_h

namespace vox {
namespace pbd {
/** Factory for unique ids.
 */
class IDFactory {
private:
    /** Current id */
    static int id;
    
public:
    static int getId() {
        return id++;
    }
};

}
}

#endif /* id_factory_h */
