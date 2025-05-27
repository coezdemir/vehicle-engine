//
// Created by CemOe on 28.09.2021.
//

#include "engine/physics/windows/WDebugWindow.h"

void engine::physics::windows::WDebugWindow::init()
{

}

void engine::physics::windows::WDebugWindow::draw()
{
    if(ImGui::Begin("Debug"))
    {
        ImGuiIO& io = ImGui::GetIO();
        io.WantCaptureKeyboard = true;

        if (ImGui::TreeNode("General"))
        {
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / io.Framerate, io.Framerate);
            ImGui::Text("%d vertices, %d indices (%d triangles)",
                        io.MetricsRenderVertices, io.MetricsRenderIndices,
                        io.MetricsRenderIndices / 3);
            ImGui::Text("%d active allocations", io.MetricsActiveAllocations);

            ImGui::TreePop();
        }

        if(ImGui::TreeNode("Physics"))
        {
            if(ImGui::TreeNode("RaycastVehicle"))
            {
                if(ImGui::TreeNode("Vehicle Description"))
                {
                    ImGui::SliderFloat3("Chassis Dims", raycastVehicleDebugInfo.chassisDims, 0.f, 10.f, "%.1f");
                    ImGui::InputFloat("Chassis Mass", &raycastVehicleDebugInfo.chassisMass, 50.f, 250.f, "%.f");
                    ImGui::InputFloat("Suspension Length", &raycastVehicleDebugInfo.suspensionLength, 0.1f, 1.f, "%f");
                    ImGui::InputFloat("Suspension Strength", &raycastVehicleDebugInfo.suspensionStrength, 1000.f, 10000.f, "%.f");
                    ImGui::InputFloat("Suspension Dampening", &raycastVehicleDebugInfo.suspensionDampening, 100.f, 1000.f, "%.f");
                    ImGui::InputFloat("Acceleration Rate", &raycastVehicleDebugInfo.accelerationFactor, 1.f, 100.f, "%.f");
                    ImGui::InputFloat("Turning Rate", &raycastVehicleDebugInfo.turningRate, 1.f, 100.f, "%.f");
                    ImGui::InputFloat("Ground Friction", &raycastVehicleDebugInfo.friction, 1.f, 100.f, "%.f");
                    ImGui::SliderFloat3("Center Of Mass Offset", raycastVehicleDebugInfo.centerOfMassOffset, -2.f, 2.f, "%.1f");
                    ImGui::InputFloat("Linear Damping", &raycastVehicleDebugInfo.linearDamping, 0.1f, 1.f, "%.f");
                    ImGui::InputFloat("Angular Damping", &raycastVehicleDebugInfo.angularDamping, 0.1f, 1.f, "%.f");

                    ImGui::InputFloat("Force", &raycastVehicleDebugInfo.force, 0.1f, 10.f, "%f");
                    ImGui::SameLine();
                    if(ImGui::Button("Apply force", ImVec2(125.f, 20.f)))
                    {
                        event_clickedAddForce.fire(raycastVehicleDebugInfo.force);
                    }

                    ImGui::TreePop();
                }

                if(ImGui::TreeNode("Current State"))
                {
                    ImGui::Text("Forward Speed: %f", raycastVehicleDebugInfo.forwardSpeed);
                    ImGui::TreePop();
                }

                if(ImGui::Button("Flip car over", ImVec2(250.f, 20.f)))
                {
                    event_flipOverClicked.fire();
                }

                if(ImGui::Button("Reset", ImVec2(125.f, 20.f)))
                {
                    event_reset.fire();
                }
                ImGui::SameLine();
                if(ImGui::Button("Update", ImVec2(125.f, 20.f)))
                {
                    event_updatedRaycastVehicle.fire(raycastVehicleDebugInfo);
                }

                ImGui::TreePop();
            }

            if(ImGui::TreeNode("RaycastVehicle4W"))
            {
                if(ImGui::TreeNode("Vehicle Description"))
                {
                    ImGui::InputFloat("Chassis Mass", &raycastVehicle4WDebugInfo.chassisMass, 50.f, 250.f, "%.f");
                    ImGui::InputFloat3("Chassis Dims", raycastVehicle4WDebugInfo.chassisDims, "%.f");
                    ImGui::InputFloat3("Chassis MOI", raycastVehicle4WDebugInfo.chassisMOI, "%.f");
                    ImGui::InputFloat3("Chassis cm Offset", raycastVehicle4WDebugInfo.chassisCMOffset, "%.f");
                    ImGui::InputFloat("Wheel Mass", &raycastVehicle4WDebugInfo.wheelMass, 0.1f, 1.f, "%.1f");
                    ImGui::InputFloat("Wheel Radius", &raycastVehicle4WDebugInfo.wheelRadius, 0.05f, 0.1, "%.1f");
                    ImGui::InputFloat("Wheel Width", &raycastVehicle4WDebugInfo.wheelWidth, 0.05f, 0.1f, "%.1f");
                    ImGui::InputFloat("Wheel MOI", &raycastVehicle4WDebugInfo.wheelMOI, 0.05f, 0.1f, "%.1f");
                    ImGui::TreePop();
                }

                if(ImGui::TreeNode("Suspension"))
                {
                    ImGui::InputFloat("Max Compression", &raycastVehicle4WDebugInfo.maxCompression, 0.1f, 1.f, "%.1f");
                    ImGui::InputFloat("Max Droop", &raycastVehicle4WDebugInfo.maxDroop, 0.1f, 1.f, "%.1f");
                    ImGui::InputFloat("Spring Strength", &raycastVehicle4WDebugInfo.springStrength, 100.f, 1000.f, "%.f");
                    ImGui::InputFloat("Spring Damper Rate", &raycastVehicle4WDebugInfo.springDamperRate, 100.f, 1000.f, "%.f");
                    ImGui::InputFloat("Sprung Mass", &raycastVehicle4WDebugInfo.sprungMass, 50.f, 100.f, "%.f");
                    ImGui::InputFloat("Camber at Rest", &raycastVehicle4WDebugInfo.camberAtRest, 0.01f, 0.1f, "%.01f");
                    ImGui::InputFloat("Camber at  Max Compression", &raycastVehicle4WDebugInfo.camberAtMaxCompression, 0.01f, 0.1f, "%.01f");
                    ImGui::InputFloat("Camber at Max Droop", &raycastVehicle4WDebugInfo.camberAtMaxDroop, 0.01f, 0.1f, "%.01f");
                    ImGui::TreePop();
                }

                if(ImGui::TreeNode("Drive Sim Data"))
                {
                    ImGui::InputFloat("Engine Peak Torque", &raycastVehicle4WDebugInfo.enginPeakTorq, 50.f, 100.f, "%.f");
                    ImGui::InputFloat("Max Omega", &raycastVehicle4WDebugInfo.maxOmega, 50.f, 100.f, "%.f");
                    ImGui::InputFloat("Gear Switch Time", &raycastVehicle4WDebugInfo.gearSwitchTime, 0.05f, 0.1f, "%.1f");
                    ImGui::InputFloat("Clutch Strength", &raycastVehicle4WDebugInfo.clutchStrength, 0.5f, 1.f, "%.1f");
                    ImGui::InputFloat("Ackermann Accuracy", &raycastVehicle4WDebugInfo.ackermannAcuracy, -0.1f, 0.1f, "%.1f");
                    ImGui::InputFloat("Ackermann Front Width", &raycastVehicle4WDebugInfo.ackermannAxleSeparation, 0.1f, 0.5f, "%.1f");
                    ImGui::InputFloat("Ackermann Rear Width", &raycastVehicle4WDebugInfo.ackermannFrontWidth, 0.01f, 0.1f, "%.01f");
                    ImGui::InputFloat("Ackermann Axle Separation", &raycastVehicle4WDebugInfo.ackermannRearWidth, 0.01f, 0.1f, "%.01f");
                    ImGui::TreePop();
                }

                if(ImGui::TreeNode("Physics Materials"))
                {
                    ImGui::InputFloat3("Drivable Material", raycastVehicle4WDebugInfo.drivableMat, "%.1f");
                    ImGui::InputFloat3("Non-Drivable Material", raycastVehicle4WDebugInfo.nonDrivableMat, "%.1f");
                    ImGui::TreePop();
                }

                if(ImGui::TreeNode("Current State"))
                {
                    ImGui::Text("Forward Speed: %f", raycastVehicle4WDebugInfo.forwardSpeed);
                    ImGui::Text("Forward Speed Abs: %f", raycastVehicle4WDebugInfo.forwardSpeedAbs);
                    ImGui::Text("Sideways Speed: %f", raycastVehicle4WDebugInfo.sidewaysSpeed);
                    ImGui::Text("Sideways Speed Abs: %f", raycastVehicle4WDebugInfo.sidewaysSpeedAbs);
                    ImGui::Text("Current Gear: %i", raycastVehicle4WDebugInfo.currentGear);
                    ImGui::Text("Target Gear: %i", raycastVehicle4WDebugInfo.targetGear);
                    ImGui::Text("Is in Reverse: %d", raycastVehicle4WDebugInfo.bIsInReverse);
                    ImGui::TreePop();
                }

                if(ImGui::TreeNode("Keyboard Input"))
                {
                    ImGui::Text("Is accelerating: %d", raycastVehicle4WDebugInfo.bIsAccelerating);
                    ImGui::Text("Is braking: %d", raycastVehicle4WDebugInfo.bIsBraking);
                    ImGui::TreePop();
                }

                if(ImGui::Button("Update", ImVec2(250.f, 20.f)))
                {
                    event_updatedRaycastVehicle4W.fire(raycastVehicle4WDebugInfo);
                }

                ImGui::TreePop();
            }

            if(ImGui::TreeNode("Tank"))
            {
                if(ImGui::TreeNode("Vehicle Description"))
                {
                    ImGui::InputFloat("Chassis Mass", &tankDebugInfo.chassisMass, 50.f, 250.f, "%.f");
                    ImGui::InputFloat3("Chassis Dims", tankDebugInfo.chassisDims, "%.f");
                    ImGui::InputFloat3("Chassis MOI", tankDebugInfo.chassisMOI, "%.f");
                    ImGui::InputFloat3("Chassis cm Offset", tankDebugInfo.chassisCMOffset, "%.f");
                    ImGui::InputFloat("Wheel Mass", &tankDebugInfo.wheelMass, 0.1f, 1.f, "%.1f");
                    ImGui::InputFloat("Wheel Radius", &tankDebugInfo.wheelRadius, 0.05f, 0.1, "%.1f");
                    ImGui::InputFloat("Wheel Width", &tankDebugInfo.wheelWidth, 0.05f, 0.1f, "%.1f");
                    ImGui::InputFloat("Wheel MOI", &tankDebugInfo.wheelMOI, 0.05f, 0.1f, "%.1f");
                    ImGui::TreePop();
                }

                if(ImGui::TreeNode("Suspension"))
                {
                    ImGui::InputFloat("Max Compression", &tankDebugInfo.maxCompression, 0.1f, 1.f, "%.1f");
                    ImGui::InputFloat("Max Droop", &tankDebugInfo.maxDroop, 0.1f, 1.f, "%.1f");
                    ImGui::InputFloat("Spring Strength", &tankDebugInfo.springStrength, 100.f, 1000.f, "%.f");
                    ImGui::InputFloat("Spring Damper Rate", &tankDebugInfo.springDamperRate, 100.f, 1000.f, "%.f");
                    ImGui::InputFloat("Sprung Mass", &tankDebugInfo.sprungMass, 50.f, 100.f, "%.f");
                    ImGui::TreePop();
                }

                if(ImGui::TreeNode("Drive Sim Data"))
                {
                    ImGui::InputFloat("Engine Peak Torque", &tankDebugInfo.enginPeakTorq, 50.f, 100.f, "%.f");
                    ImGui::InputFloat("Max Omega", &tankDebugInfo.maxOmega, 50.f, 100.f, "%.f");
                    ImGui::InputFloat("Gear Switch Time", &tankDebugInfo.gearSwitchTime, 0.05f, 0.1f, "%.1f");
                    ImGui::InputFloat("Clutch Strength", &tankDebugInfo.clutchStrength, 0.5f, 1.f, "%.1f");
                    ImGui::InputFloat("Ackermann Accuracy", &tankDebugInfo.ackermannAcuracy, -0.1f, 0.1f, "%.1f");
                    ImGui::InputFloat("Ackermann Front Width", &tankDebugInfo.ackermannAxleSeparation, 0.1f, 0.5f, "%.1f");
                    ImGui::InputFloat("Ackermann Rear Width", &tankDebugInfo.ackermannFrontWidth, 0.01f, 0.1f, "%.01f");
                    ImGui::InputFloat("Ackermann Axle Separation", &tankDebugInfo.ackermannRearWidth, 0.01f, 0.1f, "%.01f");
                    ImGui::TreePop();
                }

                if(ImGui::TreeNode("Physics Materials"))
                {
                    ImGui::InputFloat3("Drivable Material", tankDebugInfo.drivableMat, "%.1f");
                    ImGui::InputFloat3("Non-Drivable Material", tankDebugInfo.nonDrivableMat, "%.1f");
                    ImGui::TreePop();
                }

                if(ImGui::TreeNode("Current State"))
                {
                    ImGui::Text("Forward Speed: %f", tankDebugInfo.forwardSpeed);
                    ImGui::Text("Forward Speed Abs: %f", tankDebugInfo.forwardSpeedAbs);
                    ImGui::Text("Sideways Speed: %f", tankDebugInfo.sidewaysSpeed);
                    ImGui::Text("Sideways Speed Abs: %f", tankDebugInfo.sidewaysSpeedAbs);
                    ImGui::Text("Current Gear: %i", tankDebugInfo.currentGear);
                    ImGui::Text("Target Gear: %i", tankDebugInfo.targetGear);
                    ImGui::Text("Is in Reverse: %d", tankDebugInfo.bIsInReverse);
                    ImGui::TreePop();
                }

                if(ImGui::TreeNode("Keyboard Input"))
                {
                    ImGui::Text("Is accelerating: %d", tankDebugInfo.bIsAccelerating);
                    ImGui::Text("Left Thrust: %d", tankDebugInfo.bThrustLeft);
                    ImGui::Text("Right Thrust: %d", tankDebugInfo.bThrustRight);
                    ImGui::Text("Is braking: %d", tankDebugInfo.bIsBraking);
                    ImGui::Text("Is braking left: %d", tankDebugInfo.bBreakLeft);
                    ImGui::Text("Is braking right: %d", tankDebugInfo.bBreakRight);
                    ImGui::TreePop();
                }

                if(ImGui::Button("Update", ImVec2(250.f, 20.f)))
                {
                    event_updatedTank.fire(tankDebugInfo);
                }

                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
        ImGui::End();
    }
    else
        ImGui::End();
}

void engine::physics::windows::WDebugWindow::initRaycastVehicleDebugInfo(
        engine::physics::vehicle::DebugRaycastVehicle& newRaycastVehicleDebugInfo)
{
    raycastVehicleDebugInfo = newRaycastVehicleDebugInfo;
}

void engine::physics::windows::WDebugWindow::initRaycastVehicleDebugInfo(
        engine::physics::vehicle::DebugRaycastVehicle4W& newRaycastVehicleDebugInfo)
{
    raycastVehicle4WDebugInfo = newRaycastVehicleDebugInfo;
}

void engine::physics::windows::WDebugWindow::initTankVehicleDebugInfo(
        engine::physics::vehicle::DebugVehicleTank& newTankDebugInfo)
{
    tankDebugInfo = newTankDebugInfo;
}

void
engine::physics::windows::WDebugWindow::updateRaycastVehicleInfo(const engine::physics::vehicle::DebugRaycastVehicle& updated)
{
    raycastVehicleDebugInfo.forwardSpeed = updated.forwardSpeed;
    raycastVehicleDebugInfo.turningRate = updated.turningRate;
}

