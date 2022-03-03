//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "compute_subpass.h"
#include <glog/logging.h>

namespace vox {
Subpass::Type ComputeSubpass::type() {
    return Subpass::Type::Compute;
}

ComputeSubpass::ComputeSubpass(RenderContext *context,
                               Scene *scene,
                               Camera *camera) :
Subpass(context, scene, camera) {
}

void ComputeSubpass::uploadUniforms(MTL::ComputeCommandEncoder &commandEncoder,
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

void ComputeSubpass::process(const ShaderUniform &uniform, const std::any &a,
                             MTL::ComputeCommandEncoder &encoder) {
    const auto &any_uploader = _scene->computeUploader();
    
    if (const auto it = any_uploader.find(std::type_index(a.type())); it != any_uploader.cend()) {
        it->second(a, uniform.location, encoder);
    } else {
        LOG(INFO) << "Unregistered type " << std::quoted(a.type().name());
    }
}

}
