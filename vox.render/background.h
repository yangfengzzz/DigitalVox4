//
//  background.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/15.
//

#ifndef background_hpp
#define background_hpp

#include "sky/sky.h"
#include "ImathColor.h"

namespace vox {
/**
 * The Background mode enumeration.
 */
struct BackgroundMode {
    enum Enum {
        /* Solid color. */
        SolidColor,
        /* Sky. */
        Sky,
        /** Texture */
        Texture
    };
};

/**
 * Background of scene.
 */
class Background {
public:
    /**
     * Background mode.
     * @defaultValue `BackgroundMode.SolidColor`
     * @remarks If using `BackgroundMode.Sky` mode and material or mesh of the `sky` is not defined, it will downgrade to `BackgroundMode.SolidColor`.
     */
    BackgroundMode::Enum mode = BackgroundMode::Enum::SolidColor;
    
    /**
     * Background solid color.
     * @defaultValue `new Color(0.25, 0.25, 0.25, 1.0)`
     * @remarks When `mode` is `BackgroundMode.SolidColor`, the property will take effects.
     */
    Imath::Color4f solidColor = Imath::Color4f(0.25, 0.25, 0.25, 1.0);
    
    /**
     * Background sky.
     * @remarks When `mode` is `BackgroundMode.Sky`, the property will take effects.
     */
    Sky sky = Sky();
    
    Background() = default;
};

}

#endif /* background_hpp */
