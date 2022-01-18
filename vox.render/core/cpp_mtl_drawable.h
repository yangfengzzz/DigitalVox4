//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef cpp_mtl_Drawable_hpp
#define cpp_mtl_Drawable_hpp

#include "cpp_mtl_implementation.h"
#include "cpp_mtl_pixel_format.h"
#include "cpp_mtl_types.h"
#include "cpp_mtl_texture.h"

namespace MTL {
class Drawable {
public:
    Drawable() = delete;
    
    Drawable(const Drawable &rhs);
    
    Drawable(Drawable &&rhs);
    
    Drawable &operator=(const Drawable &rhs);
    
    Drawable &operator=(Drawable &&rhs);
    
    CPP_METAL_VIRTUAL ~Drawable();
    
    Texture *texture();
    
    void present();
    
private:
    cpp_mtl_internal::Drawable m_objCObj;
    
    Device *m_device;
    
    MTL::Texture *m_texture;
    
public: // Public methods for cpp_mtl_ internal implementation
    Drawable(cpp_mtl_internal::Drawable objCObj, Device &device);
    
    cpp_mtl_internal::Drawable objCObj() const;
    
    void invalidate();
};


#pragma mark - Library inline method implementations

inline Drawable::Drawable(Drawable &&rhs)
: m_objCObj(rhs.m_objCObj), m_device(rhs.m_device), m_texture(rhs.m_texture) {
    rhs.m_objCObj = nullptr;
}

inline Drawable &Drawable::operator=(Drawable &&rhs) {
    m_objCObj = rhs.m_objCObj;
    m_device = rhs.m_device;
    m_texture = rhs.m_texture;
    rhs.m_objCObj = nullptr;
    rhs.m_device = nullptr;
    rhs.m_texture = nullptr;
    return *this;
}

inline cpp_mtl_internal::Drawable Drawable::objCObj() const {
    return m_objCObj;
}


} // namespace MTL

#endif // cpp_mtl_Drawable_hpp
