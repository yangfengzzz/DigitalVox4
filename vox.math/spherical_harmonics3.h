//
//  spherical_harmonics3.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/6.
//

#ifndef spherical_harmonics3_hpp
#define spherical_harmonics3_hpp

#include "vector3.h"
#include "vector4.h"
#include <array>

namespace vox {
/**
 * Use SH3 to represent irradiance environment maps efficiently, allowing for interactive rendering of diffuse objects under distant illumination.
 * @remarks
 * https://graphics.stanford.edu/papers/envmap/envmap.pdf
 * http://www.ppsloan.org/publications/StupidSH36.pdf
 * https://google.github.io/filament/Filament.md.html#annex/sphericalharmonics
 */
class SphericalHarmonics3 {
public:
    SphericalHarmonics3();
    
    SphericalHarmonics3(std::array<float, 27> coefficients);
    
    const std::array<float, 27>& coefficients() const;
    
    /**
     * Add light to SphericalHarmonics3.
     * @param direction - Light direction
     * @param color - Light color
     * @param deltaSolidAngle - The delta solid angle of the light
     */
    void addLight(const Vector3F& direction, const Color4F& color, float deltaSolidAngle);
    
    /**
     * Evaluates the color for the specified direction.
     * @param direction - Specified direction
     * @return out - Out color
     */
    Color4F operator()(const Vector3F& direction);
    
    /**
     * Scale the coefficients.
     * @param s - The amount by which to scale the SphericalHarmonics3
     */
    SphericalHarmonics3 operator*(float s);
    
private:
  /** The coefficients of SphericalHarmonics3. */
    std::array<float, 27> _coefficients;
};

}

#endif /* spherical_harmonics3_hpp */
