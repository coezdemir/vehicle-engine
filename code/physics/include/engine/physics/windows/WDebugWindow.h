//
// Created by CemOe on 28.09.2021.
//
#pragma once
#include "engine/ui/windows/WWindow.h"
#include "engine/physics/vehicle/VehicleUtil.h"

namespace engine::physics::windows
{
    class WDebugWindow final : public engine::ui::windows::WWindow
    {
    public:
        WDebugWindow() = default;

        void initRaycastVehicleDebugInfo(engine::physics::vehicle::DebugRaycastVehicle& newRaycastVehicleDebugInfo);

        void initRaycastVehicleDebugInfo(engine::physics::vehicle::DebugRaycastVehicle4W& newRaycastVehicleDebugInfo);

        void initTankVehicleDebugInfo(engine::physics::vehicle::DebugVehicleTank& newTankDebugInfo);

    private:
        void init() override;

    public:
        void draw() override;

        void updateRaycastVehicleInfo(const engine::physics::vehicle::DebugRaycastVehicle& updatedDebugInfo);

        using event_t_addForceClicked = typename engine::callbacks::Event<WDebugWindow, float>;
        event_t_addForceClicked event_clickedAddForce;

        using event_t_void =
        typename engine::callbacks::Event<WDebugWindow>;
        event_t_void event_reset;

        event_t_void event_flipOverClicked;

        using event_t_debugRaycastVehicle =
        typename engine::callbacks::Event<WDebugWindow, engine::physics::vehicle::DebugRaycastVehicle&>;
        event_t_debugRaycastVehicle event_updatedRaycastVehicle;

        using event_t_debugRaycastVehicle4W =
        typename engine::callbacks::Event<WDebugWindow, engine::physics::vehicle::DebugRaycastVehicle4W&>;
        event_t_debugRaycastVehicle4W event_updatedRaycastVehicle4W;

        using event_t_debugTank =
        typename engine::callbacks::Event<WDebugWindow, engine::physics::vehicle::DebugVehicleTank&>;
        event_t_debugTank event_updatedTank;

        engine::physics::vehicle::DebugRaycastVehicle raycastVehicleDebugInfo;

        engine::physics::vehicle::DebugRaycastVehicle4W raycastVehicle4WDebugInfo;

        engine::physics::vehicle::DebugVehicleTank tankDebugInfo;
    };
}


