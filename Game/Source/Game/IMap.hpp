#pragma once

#include <Core/Common.hpp>
#include <Scene/Scene.hpp>

namespace Hyper
{
    class IMap
    {
    private:
    public:
        IMap() = default;
        virtual ~IMap() = default;

        virtual void Load(Scene* scene) = 0;
    };
}