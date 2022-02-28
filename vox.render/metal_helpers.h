//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef SRC_VOX_METAL_HELPERS_H_
#define SRC_VOX_METAL_HELPERS_H_

#ifndef CLONE_METAL_CUSTOM_DELETER
#   define CLONE_METAL_CUSTOM_DELETER(ClassName, ptr) \
        std::shared_ptr<ClassName>( \
            ptr, \
            [] (ClassName* obj) { \
                obj->release(); \
            });
#endif

#endif  // SRC_VOX_METAL_HELPERS_H_
