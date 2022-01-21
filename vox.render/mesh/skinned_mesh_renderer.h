//
//  skinned_mesh_renderer.hpp
//  vox.render
//
//  Created by 杨丰 on 2021/12/1.
//

#ifndef skinned_mesh_renderer_hpp
#define skinned_mesh_renderer_hpp

#include "renderer.h"
#include "loader/fbx_mesh.h"
#include <ozz/animation/runtime/skeleton.h>
#include <ozz/base/containers/vector.h>
#include <ozz/base/maths/soa_transform.h>
#include "core/cpp_mtl.h"

namespace vox {
class SkinnedMeshRenderer : public Renderer {
public:
    SkinnedMeshRenderer(Entity *entity);
    
    void _render(std::vector<RenderElement> &opaqueQueue,
                 std::vector<RenderElement> &alphaTestQueue,
                 std::vector<RenderElement> &transparentQueue) override;
    
    void _updateBounds(BoundingBox3F &worldBounds) override;
    
    void update(float deltaTime) override;
    
    bool loadSkeleton(const std::string &filename);
    
    bool addSkinnedMesh(const std::string &skin_filename,
                        const std::string &skel_filename);
    
public:
    int numJoints();
    
    int numSoaJoints();
    
private:
    // Computes the bounding box of _skeleton. This is the box that encloses all
    // skeleton's joints in model space.
    // _bound must be a valid math::Box instance.
    static void computeSkeletonBounds(const ozz::animation::Skeleton &_skeleton,
                                      BoundingBox3F *_bound);
    
    // Computes the bounding box of posture defines be _matrices range.
    // _bound must be a valid math::Box instance.
    static void computePostureBounds(ozz::span<const ozz::math::Float4x4> _matrices,
                                     BoundingBox3F *_bound);
    
    // Renders a skinned mesh at a specified location.
    std::shared_ptr<Mesh> drawSkinnedMesh(size_t index,
                                          const ozz::loader::Mesh &_mesh,
                                          const ozz::span<ozz::math::Float4x4> _skinning_matrices,
                                          const ozz::math::Float4x4 &_transform);
    
private:
    Animator *animator = nullptr;
    
    // Runtime skeleton.
    ozz::animation::Skeleton skeleton_;
    
    // Blending job bind pose threshold.
    float threshold_;
    
    // Buffer of local transforms which stores the blending result.
    ozz::vector<ozz::math::SoaTransform> blended_locals_;
    
    // Buffer of model space matrices. These are computed by the local-to-model
    // job after the blending stage.
    ozz::vector<ozz::math::Float4x4> models_;
    
    // Buffer of skinning matrices, result of the joint multiplication of the
    // inverse bind pose with the model space matrix.
    ozz::vector<ozz::math::Float4x4> skinning_matrices_;
    
    // The mesh used by the sample.
    ozz::vector<ozz::loader::Mesh> meshes_;
    ozz::vector<MTL::Buffer*> vertexBuffers;
    ozz::vector<MTL::Buffer*> uvBuffers;
    ozz::vector<MTL::Buffer*> indexBuffers;
};

}

#endif /* skinned_mesh_renderer_hpp */
