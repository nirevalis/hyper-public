#pragma once

#include <Core/Common.hpp>
#include <Game/IMap.hpp>
#include <Renderer/Assets/ModelAsset.hpp>

namespace Hyper
{
    class GymMap : public IMap
    {
    public:
        GymMap() = default;
        ~GymMap() = default;

        void Load(Scene *scene) override;
    };
}
