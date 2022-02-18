//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cloth_ui.h"
#include "cloth_controller.h"
#include "gui/imgui.h"
#include "debug_frame_manager.h"
#include <NvCloth/Factory.h>

namespace vox {
namespace cloth {
unsigned int ClothUI::_debugVisualizationFlags = 0;
bool ClothUI::_debugVisualizationUpdateRequested = true;
ClothUI::SceneDebugRenderParams ClothUI::_sceneDebugRenderParams;

void ClothUI::onUpdate() {
    _updateClothUI();
    _updateSolverUI();
    _updateDebugUI();
    
    _drawDebugVisualization();
}

void ClothUI::_updateClothUI() {
    static int activeCloth = 0;
    
    const auto controller = ClothController::getSingletonPtr();
    if (ImGui::TreeNode("Cloth Properties")) {
        activeCloth = std::min(activeCloth, (int) controller->_clothList.size() - 1);
        for (int i = 0; i < (int) controller->_clothList.size(); i++) {
            if (i)
                ImGui::SameLine();
            std::stringstream clothName;
            clothName << "Cloth " << i;
            ImGui::RadioButton(clothName.str().c_str(), &activeCloth, i);
        }
        
        nv::cloth::Cloth *cloth = controller->_clothList[activeCloth]->cloth;
        {
            bool b = cloth->isContinuousCollisionEnabled();
            if (ImGui::Checkbox("Continuous Collision Detection (CCD)", &b))
                cloth->enableContinuousCollision(b);
        }
        {
            physx::PxVec3 f3 = cloth->getDamping();
            if (ImGui::DragFloat3("Damping", &f3.x, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setDamping(f3);
            float f = f3.maxElement();
            if (ImGui::DragFloat("Damping xyz", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setDamping(physx::PxVec3(f, f, f));
        }
        {
            float f = cloth->getDragCoefficient();
            if (ImGui::DragFloat("Drag Coefficient", &f, 0.02f, 0.0f, 0.99f, "%.2f"))
                cloth->setDragCoefficient(f);
        }
        {
            float f = cloth->getFriction();
            if (ImGui::DragFloat("Friction", &f, 0.04f, 0.0f, 2.0f, "%.2f"))
                cloth->setFriction(f);
        }
        {
            physx::PxVec3 f3 = cloth->getGravity();
            if (ImGui::DragFloat3("Gravity", &f3.x, 0.5f, -50.0f, 50.0f, "%.1f"))
                cloth->setGravity(f3);
        }
        {
            float f = cloth->getLiftCoefficient();
            if (ImGui::DragFloat("Lift Coefficient", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setLiftCoefficient(f);
        }
        {
            physx::PxVec3 f3 = cloth->getLinearInertia();
            if (ImGui::DragFloat3("Linear Inertia", &f3.x, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setLinearInertia(f3);
            float f = f3.maxElement();
            if (ImGui::DragFloat("Linear Inertia xyz", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setLinearInertia(physx::PxVec3(f, f, f));
        }
        {
            physx::PxVec3 f3 = cloth->getAngularInertia();
            if (ImGui::DragFloat3("Angular Inertia", &f3.x, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setAngularInertia(f3);
            float f = f3.maxElement();
            if (ImGui::DragFloat("Angular Inertia xyz", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setAngularInertia(physx::PxVec3(f, f, f));
        }
        {
            physx::PxVec3 f3 = cloth->getCentrifugalInertia();
            if (ImGui::DragFloat3("Centrifugal Inertia", &f3.x, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setCentrifugalInertia(f3);
            float f = f3.maxElement();
            if (ImGui::DragFloat("Centrifugal Inertia xyz", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setCentrifugalInertia(physx::PxVec3(f, f, f));
        }
        {
            physx::PxVec3 f3 = cloth->getLinearDrag();
            if (ImGui::DragFloat3("Linear Drag", &f3.x, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setLinearDrag(f3);
            float f = f3.maxElement();
            if (ImGui::DragFloat("Linear Drag xyz", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setLinearDrag(physx::PxVec3(f, f, f));
        }
        {
            physx::PxVec3 f3 = cloth->getAngularDrag();
            if (ImGui::DragFloat3("Angular Drag", &f3.x, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setAngularDrag(f3);
            float f = f3.maxElement();
            if (ImGui::DragFloat("Angular Drag xyz", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setAngularDrag(physx::PxVec3(f, f, f));
        }
        {
            float f = cloth->getMotionConstraintScale();
            if (ImGui::DragFloat("Motion Constraint Scale", &f, 0.08f, 0.0f, 4.0f, "%.2f"))
                cloth->setMotionConstraintScaleBias(f, cloth->getMotionConstraintBias());
        }
        {
            float f = cloth->getMotionConstraintBias();
            if (ImGui::DragFloat("Motion Constraint Bias", &f, 0.16f, 0.0f, 8.0f, "%.2f"))
                cloth->setMotionConstraintScaleBias(cloth->getMotionConstraintScale(), f);
        }
        {
            float f = cloth->getSelfCollisionDistance();
            if (ImGui::DragFloat("Self Collision Distance", &f, 0.005f, 0.0f, 0.3f, "%.3f"))
                cloth->setSelfCollisionDistance(f);
        }
        {
            float f = cloth->getSelfCollisionStiffness();
            if (ImGui::DragFloat("Self Collision Stiffness", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setSelfCollisionStiffness(f);
        }
        {
            float f = cloth->getSleepThreshold();
            if (ImGui::DragFloat("Sleep Threshold", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setSleepThreshold(f);
        }
        {
            float f = cloth->getStiffnessFrequency();
            if (ImGui::DragFloat("Stiffness Frequency", &f, 1.0f, 0.0f, 600.0f, "%.0f", ImGuiSliderFlags_Logarithmic))
                cloth->setStiffnessFrequency(f);
        }
        {
            float f = cloth->getSolverFrequency();
            if (ImGui::DragFloat("Solver Frequency", &f, 1.0f, 0.0f, 600.0f, "%.0f", ImGuiSliderFlags_Logarithmic))
                cloth->setSolverFrequency(f);
        }
        {
            float f = cloth->getTetherConstraintScale();
            if (ImGui::DragFloat("Tether Constraint Scale", &f, 0.08f, 0.0f, 4.0f, "%.2f"))
                cloth->setTetherConstraintScale(f);
        }
        {
            float f = cloth->getTetherConstraintStiffness();
            if (ImGui::DragFloat("Tether Constraint Stiffness", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setTetherConstraintStiffness(f);
        }
        {
            physx::PxVec3 f3 = cloth->getWindVelocity();
            if (ImGui::DragFloat3("Wind Velocity", &f3.x, 0.5f, -50.0f, 50.0f, "%.1f"))
                cloth->setWindVelocity(f3);
        }
        ImGui::TreePop();
    }
}

void ClothUI::_updateSolverUI() {
    static int activeSolver = 0;
    
    const auto controller = ClothController::getSingletonPtr();
    if (ImGui::TreeNode("Solver Properties")) {
        activeSolver = std::min(activeSolver, (int) controller->_solverList.size() - 1);
        for (int i = 0; i < (int) controller->_solverList.size(); i++) {
            if (i)
                ImGui::SameLine();
            std::stringstream clothName;
            clothName << "Solver " << i;
            ImGui::RadioButton(clothName.str().c_str(), &activeSolver, i);
        }
        
        nv::cloth::Solver *solver = controller->_solverList[activeSolver];
        
        {
            float f = solver->getInterCollisionDistance();
            if (ImGui::DragFloat("Inter Collision Distance", &f, 0.005f, 0.0f, 2.0f, "%.2f"))
                solver->setInterCollisionDistance(f);
        }
        {
            uint32_t i = solver->getInterCollisionNbIterations();
            if (ImGui::DragInt("Inter Collision Iterations", (int *) &i, 0.25, 0, 16))
                solver->setInterCollisionNbIterations(i);
        }
        {
            float f = solver->getInterCollisionStiffness();
            if (ImGui::DragFloat("Inter Collision Stiffness", &f, 0.005f, 0.0f, 1.0f, "%.2f"))
                solver->setInterCollisionStiffness(f);
        }
        ImGui::TreePop();
    }
}

//MARK: - Debug
void ClothUI::_updateDebugUI() {
    if (ImGui::TreeNode("Debug Visualization")) {
        auto old = _debugVisualizationFlags;
        ImGui::CheckboxFlags("Tethers (T)", &_debugVisualizationFlags, DEBUG_VIS_TETHERS);
        ImGui::CheckboxFlags("Constraints (C)", &_debugVisualizationFlags, DEBUG_VIS_CONSTRAINTS);
        if (_debugVisualizationFlags & DEBUG_VIS_CONSTRAINTS) {
            ImGui::DragInt("Start Constraint Phase Range", &_sceneDebugRenderParams.visiblePhaseRangeBegin, 0.05, 0, 30);
            ImGui::DragInt("End", &_sceneDebugRenderParams.visiblePhaseRangeEnd, 0.05, 0, 30);
        }
        ImGui::CheckboxFlags("Constraint Stiffness (F)", &_debugVisualizationFlags, DEBUG_VIS_CONSTRAINTS_STIFFNESS);
        ImGui::CheckboxFlags("Constraint Error (R)", &_debugVisualizationFlags, DEBUG_VIS_CONSTRAINT_ERROR);
        ImGui::CheckboxFlags("Position Delta (L)", &_debugVisualizationFlags, DEBUG_VIS_POSITION_DELTA);
        ImGui::CheckboxFlags("Bounding Box (X)", &_debugVisualizationFlags, DEBUG_VIS_BOUNDING_BOX);
        ImGui::CheckboxFlags("Distance Constraints (Z)", &_debugVisualizationFlags, DEBUG_VIS_DISTANCE_CONSTRAINTS);
        ImGui::TreePop();
        
        if (old != _debugVisualizationFlags)
            _debugVisualizationUpdateRequested = true;
    }
}

void ClothUI::_drawDebugVisualization() {
    if (_debugVisualizationFlags & DEBUG_VIS_TETHERS)
        _debugRenderTethers();
    if (_debugVisualizationFlags & DEBUG_VIS_CONSTRAINTS)
        _debugRenderConstraints();
    if (_debugVisualizationFlags & DEBUG_VIS_CONSTRAINTS_STIFFNESS)
        _debugRenderConstraintStiffness();
    
    if (_debugVisualizationFlags & DEBUG_VIS_CONSTRAINT_ERROR)
        _debugRenderConstraintError();
    if (_debugVisualizationFlags & DEBUG_VIS_POSITION_DELTA)
        _debugRenderPositionDelta();
    if (_debugVisualizationFlags & DEBUG_VIS_BOUNDING_BOX)
        _debugRenderBoundingBox();
    if (_debugVisualizationFlags & DEBUG_VIS_DISTANCE_CONSTRAINTS)
        _debugRenderDistanceConstraints();
    
    _debugVisualizationUpdateRequested = false;
}


void ClothUI::_debugRenderDistanceConstraints() {
    const auto controller = ClothController::getSingletonPtr();
    const auto debugMananger = DebugFrameManager::getSingletonPtr();
    
    for (auto it: controller->_clothList) {
        nv::cloth::Cloth &cloth = *it->cloth;
        if (cloth.getNumMotionConstraints() == 0)
            continue;
        
        nv::cloth::Factory &factory = cloth.getFactory();
        
        Matrix4x4F transform = it->clothRenderer->entity()->transform->worldMatrix();
        
        nv::cloth::MappedRange<physx::PxVec4> particles = cloth.getCurrentParticles();
        std::vector<physx::PxVec4> motionConstraints;
        motionConstraints.reserve(cloth.getNumMotionConstraints() * 2);
        motionConstraints.resize(cloth.getNumMotionConstraints());
        factory.extractMotionConstraints(cloth, nv::cloth::Range<physx::PxVec4>(&motionConstraints[0], &motionConstraints[0] + motionConstraints.size()));
        motionConstraints.resize(cloth.getNumMotionConstraints() * 2);
        
        nv::cloth::MappedRange<physx::PxVec4> positions = cloth.getCurrentParticles();
        
        assert(positions.size() == cloth.getNumMotionConstraints());
        
        
        //Set to 1 to color code lines based on distance constraint length (as % of max constraint distance in cloth)
        //Set to 0 to color code lines based on how close the particle is to the distance constraint (as % of max distance per constraint)
#define SHOW_DISTANCE_COLOR 0
#if SHOW_DISTANCE_COLOR
        float maxDist = 0.0f;
        for(int i = (int)(motionConstraints.size() >> 1) - 1; i >= 0; i--)
        {
            maxDist = max(maxDist, motionConstraints[i].w);
        }
#endif
        
        for (int i = ((int) motionConstraints.size() >> 1) - 1; i >= 0; i--) {
            float l = motionConstraints[i].w;
            physx::PxVec3 a = motionConstraints[i].getXYZ();
            physx::PxVec3 b = positions[i].getXYZ();
            physx::PxVec3 d = b - a;
            float currentDist = d.magnitude();
            if (d.magnitudeSquared() < 0.00001f) {
                d = physx::PxVec3(0.0f, 0.0f, 1.0f);
            } else {
                d.normalize();
            }
            unsigned char clerp;
#if SHOW_DISTANCE_COLOR
            clerp = (unsigned char)(std::max(0.0f,std::min(1.0f,(l / maxDist)))*255.0f);
#else
            clerp = (unsigned char) (std::max(0.0f, std::min(1.0f, (currentDist / l))) * 255.0f);
#endif
            unsigned int c = ((255 - clerp) << 8) + clerp;
            
            physx::PxVec3 temp = a + d * l;
            debugMananger->addLine(transform, Vector3F(a.x, a.y, a.z), Vector3F(temp.x, temp.y, temp.z), c);
        }
    }
}

void ClothUI::_debugRenderTethers() {
    const auto controller = ClothController::getSingletonPtr();
    const auto debugMananger = DebugFrameManager::getSingletonPtr();
    
    for (auto it: controller->_clothList) {
        nv::cloth::Cloth &cloth = *it->cloth;
        nv::cloth::Fabric &fabric = cloth.getFabric();
        if (fabric.getNumTethers() == 0)
            continue;
        
        nv::cloth::Factory &factory = cloth.getFactory();
        
        Matrix4x4F transform = it->clothRenderer->entity()->transform->worldMatrix();
        
        nv::cloth::MappedRange<physx::PxVec4> particles = cloth.getCurrentParticles();
        
        std::vector<float> tetherLengths;
        tetherLengths.resize(fabric.getNumTethers());
        std::vector<uint32_t> anchors;
        anchors.resize(fabric.getNumTethers());
        
        factory.extractFabricData(fabric, nv::cloth::Range<uint32_t>(0, 0), nv::cloth::Range<uint32_t>(0, 0),
                                  nv::cloth::Range<float>(0, 0), nv::cloth::Range<float>(0, 0),
                                  nv::cloth::Range<uint32_t>(0, 0),
                                  nv::cloth::Range<uint32_t>(&anchors[0], &anchors[0] + anchors.size()),
                                  nv::cloth::Range<float>(&tetherLengths[0], &tetherLengths[0] + tetherLengths.size()),
                                  nv::cloth::Range<uint32_t>(0, 0));
        
        int particleCount = fabric.getNumParticles();
        
        for (int i = 0; i < (int) anchors.size(); i++) {
            float lengthDiff = (particles[anchors[i]].getXYZ() - particles[i].getXYZ()).magnitude() - tetherLengths[i];
            
            physx::PxVec3 p0Temp = particles[anchors[i]].getXYZ();
            physx::PxVec3 p1Temp = particles[i % particleCount].getXYZ();
            debugMananger->addLine(transform, Vector3F(p0Temp.x, p0Temp.y, p0Temp.z),
                                   Vector3F(p1Temp.x, p1Temp.y, p1Temp.z), lengthDiff > 0.0f ? 0x0000FF : 0x00FFFF);
        }
    }
}

void ClothUI::_debugRenderConstraints() {
    const auto controller = ClothController::getSingletonPtr();
    const auto debugMananger = DebugFrameManager::getSingletonPtr();
    
    for (auto it: controller->_clothList) {
        nv::cloth::Cloth &cloth = *it->cloth;
        nv::cloth::Fabric &fabric = cloth.getFabric();
        if (fabric.getNumIndices() == 0)
            continue;
        
        nv::cloth::Factory &factory = cloth.getFactory();
        
        Matrix4x4F transform = it->clothRenderer->entity()->transform->worldMatrix();
        
        nv::cloth::MappedRange<physx::PxVec4> particles = cloth.getCurrentParticles();
        
        if (_sceneDebugRenderParams.visiblePhaseRangeBegin >= _sceneDebugRenderParams.visiblePhaseRangeEnd) {
            //then simply render all constraints in one go
            std::vector<uint32_t> indices;
            indices.resize(fabric.getNumIndices());
            
            factory.extractFabricData(fabric, nv::cloth::Range<uint32_t>(0, 0), nv::cloth::Range<uint32_t>(0, 0),
                                      nv::cloth::Range<float>(0, 0), nv::cloth::Range<float>(0, 0),
                                      nv::cloth::Range<uint32_t>(&indices[0], &indices[0] + indices.size()),
                                      nv::cloth::Range<uint32_t>(0, 0), nv::cloth::Range<float>(0, 0), nv::cloth::Range<uint32_t>(0, 0));
            
            for (int i = 1; i < (int) indices.size(); i += 2) {
                physx::PxVec3 p0Temp = particles[indices[i]].getXYZ();
                physx::PxVec3 p1Temp = particles[indices[i - 1]].getXYZ();
                debugMananger->addLine(transform, Vector3F(p0Temp.x, p0Temp.y, p0Temp.z),
                                       Vector3F(p1Temp.x, p1Temp.y, p1Temp.z), 0x991919);
            }
        } else {
            //otherwise we render individual phases
            std::vector<uint32_t> indices;
            indices.resize(fabric.getNumIndices());
            std::vector<uint32_t> phases;
            phases.resize(fabric.getNumPhases());
            std::vector<uint32_t> sets;
            sets.resize(fabric.getNumSets());
            
            factory.extractFabricData(fabric, nv::cloth::Range<uint32_t>(&phases[0], &phases[0] + phases.size()),
                                      nv::cloth::Range<uint32_t>(&sets[0], &sets[0] + sets.size()),
                                      nv::cloth::Range<float>(0, 0), nv::cloth::Range<float>(0, 0),
                                      nv::cloth::Range<uint32_t>(&indices[0], &indices[0] + indices.size()),
                                      nv::cloth::Range<uint32_t>(0, 0), nv::cloth::Range<float>(0, 0), nv::cloth::Range<uint32_t>(0, 0));
            
            uint32_t *iIt = &indices[0];
            for (int phaseIndex = 0; phaseIndex < (int) fabric.getNumPhases(); phaseIndex++) {
                uint32_t *sIt = &sets[phases[phaseIndex]];
                uint32_t *iEnd = &indices[0] + (sIt[0] * 2);
                uint32_t *iStart = iIt;
                
                if (!(phaseIndex >= _sceneDebugRenderParams.visiblePhaseRangeBegin && phaseIndex < _sceneDebugRenderParams.visiblePhaseRangeEnd)) {
                    iIt = iEnd;
                    continue;
                }
                
                for (iIt; iIt < iEnd; iIt += 2) {
                    float c = (float) (iIt - iStart) / (float) (iEnd - iStart);
                    unsigned char c255 = (unsigned char) (c * 255.0f);
                    
                    unsigned int colorTable[3]
                    {
                        0xFF0000,
                        0x00FF00,
                        0x0000FF
                    };
                    unsigned int shiftTable[3]
                    {
                        8,
                        0,
                        16
                    };
                    
                    physx::PxVec3 p0Temp = particles[*iIt].getXYZ();
                    physx::PxVec3 p1Temp = particles[*(iIt + 1)].getXYZ();
                    debugMananger->addLine(transform, Vector3F(p0Temp.x, p0Temp.y, p0Temp.z),
                                           Vector3F(p1Temp.x, p1Temp.y, p1Temp.z),
                                           colorTable[phaseIndex % 3] + (c255 << shiftTable[phaseIndex % 3]));
                }
            }
        }
    }
}

void ClothUI::_debugRenderConstraintStiffness() {
    const auto controller = ClothController::getSingletonPtr();
    const auto debugMananger = DebugFrameManager::getSingletonPtr();
    
    for (auto it: controller->_clothList) {
        nv::cloth::Cloth &cloth = *it->cloth;
        nv::cloth::Fabric &fabric = cloth.getFabric();
        if (fabric.getNumIndices() == 0)
            continue;
        
        if (!fabric.getNumStiffnessValues())
            continue;
        
        nv::cloth::Factory &factory = cloth.getFactory();
        
        Matrix4x4F transform = it->clothRenderer->entity()->transform->worldMatrix();
        
        nv::cloth::MappedRange<physx::PxVec4> particles = cloth.getCurrentParticles();
        
        std::vector<uint32_t> indices;
        indices.resize(fabric.getNumIndices());
        std::vector<float> stiffness;
        stiffness.resize(fabric.getNumRestvalues());
        
        factory.extractFabricData(fabric, nv::cloth::Range<uint32_t>(0, 0), nv::cloth::Range<uint32_t>(0, 0), nv::cloth::Range<float>(0, 0), nv::cloth::Range<float>(&stiffness[0], &stiffness[0] + stiffness.size()), nv::cloth::Range<uint32_t>(&indices[0], &indices[0] + indices.size()),
                                  nv::cloth::Range<uint32_t>(0, 0), nv::cloth::Range<float>(0, 0), nv::cloth::Range<uint32_t>(0, 0));
        
        for (int i = 1; i < (int) indices.size(); i += 2) {
            float c = 1.0f - exp2f(stiffness[i >> 1]);
            
            unsigned char ca255 = (unsigned char) (c * 255.0f * 0.8f);
            unsigned char cb255 = (unsigned char) ((1.0f - c) * 255.0f * 0.8f);
            
            physx::PxVec3 p0Temp = particles[indices[i - 1]].getXYZ();
            physx::PxVec3 p1Temp = particles[indices[i]].getXYZ();
            debugMananger->addLine(transform, Vector3F(p0Temp.x, p0Temp.y, p0Temp.z),
                                   Vector3F(p1Temp.x, p1Temp.y, p1Temp.z), (ca255 << 8) + (cb255 << 0));
        }
    }
}

void ClothUI::_debugRenderConstraintError() {
    const auto controller = ClothController::getSingletonPtr();
    const auto debugMananger = DebugFrameManager::getSingletonPtr();
    
    for (auto it: controller->_clothList) {
        nv::cloth::Cloth &cloth = *it->cloth;
        nv::cloth::Fabric &fabric = cloth.getFabric();
        if (fabric.getNumRestvalues() == 0) {continue;}
        nv::cloth::Factory &factory = cloth.getFactory();
        
        Matrix4x4F transform = it->clothRenderer->entity()->transform->worldMatrix();
        
        nv::cloth::MappedRange<physx::PxVec4> particles = cloth.getCurrentParticles();
        
        std::vector<uint32_t> indices;
        indices.resize(fabric.getNumIndices());
        std::vector<float> restLengths;
        restLengths.resize(fabric.getNumRestvalues());
        
        factory.extractFabricData(fabric, nv::cloth::Range<uint32_t>(0, 0),
                                  nv::cloth::Range<uint32_t>(0, 0),
                                  nv::cloth::Range<float>(&restLengths[0], &restLengths[0] + restLengths.size()),
                                  nv::cloth::Range<float>(0, 0),
                                  nv::cloth::Range<uint32_t>(&indices[0], &indices[0] + indices.size()),
                                  nv::cloth::Range<uint32_t>(0, 0), nv::cloth::Range<float>(0, 0), nv::cloth::Range<uint32_t>(0, 0));
        
        for (int i = 0; i < (int) indices.size(); i += 2) {
            physx::PxVec4 p0 = particles[indices[i]];
            physx::PxVec4 p1 = particles[indices[i + 1]];
            float restLength = restLengths[i >> 1];
            float length = (p0 - p1).magnitude();
            const float scale = 2.0f;
            float error = (length / restLength * 0.5f - 0.5f) * scale + 0.5f;
            error = std::max(0.0f, std::min(1.0f, error));
            unsigned char c255 = (unsigned char) (error * 255.0f * 0.8f);
            
            physx::PxVec3 p0Temp = p0.getXYZ();
            physx::PxVec3 p1Temp = p1.getXYZ();
            debugMananger->addLine(transform, Vector3F(p0Temp.x, p0Temp.y, p0Temp.z),
                                   Vector3F(p1Temp.x, p1Temp.y, p1Temp.z), ((255 - c255) << 8) + (c255 << 0));
        }
    }
}

void ClothUI::_debugRenderPositionDelta() {
    const auto controller = ClothController::getSingletonPtr();
    const auto debugMananger = DebugFrameManager::getSingletonPtr();
    
    for (auto it: controller->_clothList) {
        nv::cloth::Cloth &cloth = *it->cloth;
        
        Matrix4x4F transform = it->clothRenderer->entity()->transform->worldMatrix();
        
        nv::cloth::MappedRange<physx::PxVec4> particles1 = cloth.getCurrentParticles();
        nv::cloth::MappedRange<physx::PxVec4> particles0 = cloth.getPreviousParticles();
        
        std::vector<physx::PxVec4> lines;
        
        //scale so that the solver frequency doesn't affect the position delta length assuming 60fps
        float iterationsPerFrame = std::max(1, int(1.0f / 60.0f * cloth.getSolverFrequency() + 0.5f));
        
        for (int i = 0; i < (int) particles1.size(); i++) {
            physx::PxVec3 o = particles1[i].getXYZ();
            physx::PxVec3 d = (particles1[i] - particles0[i]).getXYZ();
            debugMananger->addVector(transform, Vector3F(o.x, o.y, o.z),
                                     Vector3F(d.x, d.y, d.z) * iterationsPerFrame * 2.0f,
                                     DebugFrameManager::DebugColor::RGB_RED);
        }
    }
}

void ClothUI::_debugRenderBoundingBox() {
    const auto controller = ClothController::getSingletonPtr();
    const auto debugMananger = DebugFrameManager::getSingletonPtr();
    
    for (auto it: controller->_clothList) {
        nv::cloth::Cloth &cloth = *it->cloth;
        
        Matrix4x4F transform = it->clothRenderer->entity()->transform->worldMatrix();
        
        physx::PxVec3 cTmep = cloth.getBoundingBoxCenter();
        Vector3F c = Vector3F(cTmep.x, cTmep.y, cTmep.z);
        physx::PxVec3 dTemp = cloth.getBoundingBoxScale();
        Vector3F d = Vector3F(dTemp.x, dTemp.y, dTemp.z);
        Vector3F dx = Vector3F(d.x, 0.0f, 0.0f);
        Vector3F dy = Vector3F(0.0f, d.y, 0.0f);
        Vector3F dz = Vector3F(0.0f, 0.0f, d.z);
        
        debugMananger->addLine(transform, c + dy + dz - dx, c + dy + dz + dx, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform, c + dy - dz - dx, c + dy - dz + dx, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform, c - dy + dz - dx, c - dy + dz + dx, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform, c - dy - dz - dx, c - dy - dz + dx, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform, c + dy + dx - dz, c + dy + dx + dz, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform, c + dy - dx - dz, c + dy - dx + dz, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform, c - dy + dx - dz, c - dy + dx + dz, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform, c - dy - dx - dz, c - dy - dx + dz, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform, c + dz + dx - dy, c + dz + dx + dy, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform, c + dz - dx - dy, c + dz - dx + dy, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform, c - dz + dx - dy, c - dz + dx + dy, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform, c - dz - dx - dy, c - dz - dx + dy, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform, c + dy + dz + dx, c - dy - dz - dx, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform, c + dy + dz - dx, c - dy - dz + dx, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform, c - dy + dz + dx, c + dy - dz - dx, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform, c - dy + dz - dx, c + dy - dz + dx, DebugFrameManager::DebugColor::RGB_DARKGREEN);
    }
}

}
}
