//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "render_subpass.h"
#include "material/material.h"
#include "renderer.h"
#include <glog/logging.h>

namespace vox {
Subpass::Type RenderSubpass::type() {
    return Subpass::Type::Render;
}

RenderSubpass::RenderSubpass(RenderContext *context,
                             Scene *scene,
                             Camera *camera) :
Subpass(context, scene, camera) {
}

bool RenderSubpass::_compareFromNearToFar(const RenderElement &a, const RenderElement &b) {
    return (a.material->renderQueueType < b.material->renderQueueType) ||
    (a.renderer->distanceForSort() < b.renderer->distanceForSort());
}

bool RenderSubpass::_compareFromFarToNear(const RenderElement &a, const RenderElement &b) {
    return (a.material->renderQueueType < b.material->renderQueueType) ||
    (b.renderer->distanceForSort() < a.renderer->distanceForSort());
}

void RenderSubpass::uploadUniforms(MTL::RenderCommandEncoder &commandEncoder,
                                   const std::vector<ShaderUniform> &uniformBlock,
                                   const ShaderData &shaderData) {
    const auto &properties = shaderData.properties();
    
    for (size_t i = 0; i < uniformBlock.size(); i++) {
        const auto &uniform = uniformBlock[i];
        auto iter = properties.find(uniform.propertyId);
        if (iter != properties.end()) {
            process(uniform, iter->second, commandEncoder);
        }
    }
}

void RenderSubpass::process(const ShaderUniform &uniform, const std::any &a,
                            MTL::RenderCommandEncoder &encoder) {
    const auto &any_uploader = uniform.type == MTL::FunctionTypeVertex ?
    _scene->vertexUploader() : _scene->fragmentUploader();
    
    if (const auto it = any_uploader.find(std::type_index(a.type())); it != any_uploader.cend()) {
        it->second(a, uniform.location, encoder);
    } else {
        LOG(INFO) << "Unregistered type " << std::quoted(a.type().name());
    }
}


}