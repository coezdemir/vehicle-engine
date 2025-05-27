//
// Created by CemOe on 27.09.2021.
//
#pragma once
#include <imgui.h>
#include "../UserInterface.h"

namespace engine::ui::windows
{
    /// \brief Abstract class for creating a ui window
    class WWindow
    {
    private:
        friend UserInterface;

        virtual void init() = 0;

        virtual void draw() = 0;
    };
}