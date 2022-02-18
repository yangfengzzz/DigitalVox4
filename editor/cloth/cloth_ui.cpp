//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "cloth_ui.h"
#include "cloth_controller.h"
#include "gui/imgui.h"
#include "debug_frame_manager.h"

namespace vox {
namespace cloth {
void ClothUI::onUpdate() {
    _updateClothUI();
    _updateSolverUI();
    
    _debugRenderBoundingBox();
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
            if(ImGui::Checkbox("Continuous Collision Detection (CCD)", &b))
                cloth->enableContinuousCollision(b);
        }
        {
            physx::PxVec3 f3 = cloth->getDamping();
            if(ImGui::DragFloat3("Damping", &f3.x, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setDamping(f3);
            float f = f3.maxElement();
            if(ImGui::DragFloat("Damping xyz", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setDamping(physx::PxVec3(f,f,f));
        }
        {
            float f = cloth->getDragCoefficient();
            if(ImGui::DragFloat("Drag Coefficient", &f, 0.02f, 0.0f, 0.99f, "%.2f"))
                cloth->setDragCoefficient(f);
        }
        {
            float f = cloth->getFriction();
            if(ImGui::DragFloat("Friction", &f, 0.04f, 0.0f, 2.0f, "%.2f"))
                cloth->setFriction(f);
        }
        {
            physx::PxVec3 f3 = cloth->getGravity();
            if(ImGui::DragFloat3("Gravity", &f3.x, 0.5f, -50.0f, 50.0f, "%.1f"))
                cloth->setGravity(f3);
        }
        {
            float f = cloth->getLiftCoefficient();
            if(ImGui::DragFloat("Lift Coefficient", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setLiftCoefficient(f);
        }
        {
            physx::PxVec3 f3 = cloth->getLinearInertia();
            if(ImGui::DragFloat3("Linear Inertia", &f3.x, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setLinearInertia(f3);
            float f = f3.maxElement();
            if(ImGui::DragFloat("Linear Inertia xyz", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setLinearInertia(physx::PxVec3(f, f, f));
        }
        {
            physx::PxVec3 f3 = cloth->getAngularInertia();
            if(ImGui::DragFloat3("Angular Inertia", &f3.x, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setAngularInertia(f3);
            float f = f3.maxElement();
            if(ImGui::DragFloat("Angular Inertia xyz", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setAngularInertia(physx::PxVec3(f, f, f));
        }
        {
            physx::PxVec3 f3 = cloth->getCentrifugalInertia();
            if(ImGui::DragFloat3("Centrifugal Inertia", &f3.x, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setCentrifugalInertia(f3);
            float f = f3.maxElement();
            if(ImGui::DragFloat("Centrifugal Inertia xyz", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setCentrifugalInertia(physx::PxVec3(f, f, f));
        }
        {
            physx::PxVec3 f3 = cloth->getLinearDrag();
            if(ImGui::DragFloat3("Linear Drag", &f3.x, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setLinearDrag(f3);
            float f = f3.maxElement();
            if(ImGui::DragFloat("Linear Drag xyz", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setLinearDrag(physx::PxVec3(f, f, f));
        }
        {
            physx::PxVec3 f3 = cloth->getAngularDrag();
            if(ImGui::DragFloat3("Angular Drag", &f3.x, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setAngularDrag(f3);
            float f = f3.maxElement();
            if(ImGui::DragFloat("Angular Drag xyz", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setAngularDrag(physx::PxVec3(f, f, f));
        }
        {
            float f = cloth->getMotionConstraintScale();
            if(ImGui::DragFloat("Motion Constraint Scale", &f, 0.08f, 0.0f, 4.0f, "%.2f"))
                cloth->setMotionConstraintScaleBias(f, cloth->getMotionConstraintBias());
        }
        {
            float f = cloth->getMotionConstraintBias();
            if(ImGui::DragFloat("Motion Constraint Bias", &f, 0.16f, 0.0f, 8.0f, "%.2f"))
                cloth->setMotionConstraintScaleBias(cloth->getMotionConstraintScale(),f);
        }
        {
            float f = cloth->getSelfCollisionDistance();
            if(ImGui::DragFloat("Self Collision Distance", &f, 0.005f, 0.0f, 0.3f, "%.3f"))
                cloth->setSelfCollisionDistance(f);
        }
        {
            float f = cloth->getSelfCollisionStiffness();
            if(ImGui::DragFloat("Self Collision Stiffness", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setSelfCollisionStiffness(f);
        }
        {
            float f = cloth->getSleepThreshold();
            if(ImGui::DragFloat("Sleep Threshold", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setSleepThreshold(f);
        }
        {
            float f = cloth->getStiffnessFrequency();
            if(ImGui::DragFloat("Stiffness Frequency", &f, 1.0f, 0.0f, 600.0f, "%.0f", ImGuiSliderFlags_Logarithmic))
                cloth->setStiffnessFrequency(f);
        }
        {
            float f = cloth->getSolverFrequency();
            if(ImGui::DragFloat("Solver Frequency", &f, 1.0f, 0.0f, 600.0f, "%.0f", ImGuiSliderFlags_Logarithmic))
                cloth->setSolverFrequency(f);
        }
        {
            float f = cloth->getTetherConstraintScale();
            if(ImGui::DragFloat("Tether Constraint Scale", &f, 0.08f, 0.0f, 4.0f, "%.2f"))
                cloth->setTetherConstraintScale(f);
        }
        {
            float f = cloth->getTetherConstraintStiffness();
            if(ImGui::DragFloat("Tether Constraint Stiffness", &f, 0.02f, 0.0f, 1.0f, "%.2f"))
                cloth->setTetherConstraintStiffness(f);
        }
        {
            physx::PxVec3 f3 = cloth->getWindVelocity();
            if(ImGui::DragFloat3("Wind Velocity", &f3.x, 0.5f, -50.0f, 50.0f, "%.1f"))
                cloth->setWindVelocity(f3);
        }
        ImGui::TreePop();
    }
}

void ClothUI::_updateSolverUI() {
    static int activeSolver = 0;
    
    const auto controller = ClothController::getSingletonPtr();
    if(ImGui::TreeNode("Solver Properties"))
    {
        activeSolver = std::min(activeSolver, (int)controller->_solverList.size() - 1);
        for(int i = 0; i < (int)controller->_solverList.size(); i++)
        {
            if(i)
                ImGui::SameLine();
            std::stringstream clothName;
            clothName << "Solver " << i;
            ImGui::RadioButton(clothName.str().c_str(), &activeSolver, i);
        }

        nv::cloth::Solver* solver = controller->_solverList[activeSolver];

        {
            float f = solver->getInterCollisionDistance();
            if(ImGui::DragFloat("Inter Collision Distance", &f, 0.005f, 0.0f, 2.0f, "%.2f"))
                solver->setInterCollisionDistance(f);
        }
        {
            uint32_t i = solver->getInterCollisionNbIterations();
            if(ImGui::DragInt("Inter Collision Iterations", (int*)&i, 0.25, 0, 16))
                solver->setInterCollisionNbIterations(i);
        }
        {
            float f = solver->getInterCollisionStiffness();
            if(ImGui::DragFloat("Inter Collision Stiffness", &f, 0.005f, 0.0f, 1.0f, "%.2f"))
                solver->setInterCollisionStiffness(f);
        }
        ImGui::TreePop();
    }
}

void ClothUI::_debugRenderBoundingBox() {
    const auto controller = ClothController::getSingletonPtr();
    const auto debugMananger = DebugFrameManager::getSingletonPtr();

    for(auto it : controller->_clothList) {
        nv::cloth::Cloth& cloth = *it->cloth;

        Matrix4x4F transform = it->clothRenderer->entity()->transform->worldMatrix();

        physx::PxVec3 cTmep = cloth.getBoundingBoxCenter();
        Vector3F c = Vector3F(cTmep.x, cTmep.y, cTmep.z);
        physx::PxVec3 dTemp = cloth.getBoundingBoxScale();
        Vector3F d = Vector3F(dTemp.x, dTemp.y, dTemp.z);
        Vector3F dx = Vector3F(d.x, 0.0f, 0.0f);
        Vector3F dy = Vector3F(0.0f, d.y, 0.0f);
        Vector3F dz = Vector3F(0.0f, 0.0f, d.z);

        debugMananger->addLine(transform,c + dy + dz - dx,c + dy + dz + dx, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform,c + dy - dz - dx,c + dy - dz + dx, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform,c - dy + dz - dx,c - dy + dz + dx, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform,c - dy - dz - dx,c - dy - dz + dx, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform,c + dy + dx - dz,c + dy + dx + dz, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform,c + dy - dx - dz,c + dy - dx + dz, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform,c - dy + dx - dz,c - dy + dx + dz, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform,c - dy - dx - dz,c - dy - dx + dz, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform,c + dz + dx - dy,c + dz + dx + dy, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform,c + dz - dx - dy,c + dz - dx + dy, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform,c - dz + dx - dy,c - dz + dx + dy, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform,c - dz - dx - dy,c - dz - dx + dy, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform,c + dy + dz + dx,c - dy - dz - dx, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform,c + dy + dz - dx,c - dy - dz + dx, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform,c - dy + dz + dx,c + dy - dz - dx, DebugFrameManager::DebugColor::RGB_DARKGREEN);
        debugMananger->addLine(transform,c - dy + dz - dx,c + dy - dz + dx, DebugFrameManager::DebugColor::RGB_DARKGREEN);
    }
}

}
}
