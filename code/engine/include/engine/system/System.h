// GameLab 3 Tutorial 2019/20
// Andreas Knote <andreas.knote@uni-wuerzburg.de>
#pragma once
#include "../Engine_NS.h"

namespace engine::system
{
    class System
    {
        friend class engine::Engine;

    public:
        System() = default;

        virtual ~System() = default;

        System(System&&) = delete;

        System(const System&) = delete;

    private:
        virtual void registerSystem(Engine* engine) = 0;
    };
}