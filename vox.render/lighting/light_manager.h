//
//  light_manager.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/19.
//

#ifndef light_manager_hpp
#define light_manager_hpp

#include "point_light.h"
#include "spot_light.h"
#include "direct_light.h"
#include "shader/shader_data.h"
#include "core/cpp_mtl_buffer.h"

namespace vox {
/**
 * Light Manager.
 */
class LightManager {
public:
    static constexpr uint32_t MAX_SHADOW = 10;
    static constexpr uint32_t MAX_CUBE_SHADOW = 5;
    
    LightManager();
    
    void registerUploader(Scene *scene);
    
    /**
     * Register a light object to the current scene.
     * @param light render light
     */
    void attachPointLight(PointLight *light);
    
    /**
     * Remove a light object from the current scene.
     * @param light render light
     */
    void detachPointLight(PointLight *light);
    
    const std::vector<PointLight *> &pointLights() const;
    
public:
    /**
     * Register a light object to the current scene.
     * @param light render light
     */
    void attachSpotLight(SpotLight *light);
    
    /**
     * Remove a light object from the current scene.
     * @param light render light
     */
    void detachSpotLight(SpotLight *light);
    
    const std::vector<SpotLight *> &spotLights() const;
    
public:
    /**
     * Register a light object to the current scene.
     * @param light direct light
     */
    void attachDirectLight(DirectLight *light);
    
    /**
     * Remove a light object from the current scene.
     * @param light direct light
     */
    void detachDirectLight(DirectLight *light);
    
    const std::vector<DirectLight *> &directLights() const;
    
public:
    void updateShaderData(MTL::Device* device, ShaderData &shaderData);
    
private:
    std::vector<PointLight *> _pointLights;
    std::vector<PointLightData> _pointLightDatas;
    std::shared_ptr<MTL::Buffer> _pointLightBuffer;
    ShaderProperty _pointLightProperty;
    
    std::vector<SpotLight *> _spotLights;
    std::vector<SpotLightData> _spotLightDatas;
    std::shared_ptr<MTL::Buffer> _spotLightBuffer;
    ShaderProperty _spotLightProperty;
    
    std::vector<DirectLight *> _directLights;
    std::vector<DirectLightData> _directLightDatas;
    std::shared_ptr<MTL::Buffer> _directLightBuffer;
    ShaderProperty _directLightProperty;
};

}

#endif /* light_manager_hpp */