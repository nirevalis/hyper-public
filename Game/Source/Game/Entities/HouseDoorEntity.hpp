#pragma once

#include <Core/Common.hpp>
#include <Game/ActionSystem.hpp>
#include <Scene/Entity.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>
#include <Renderer/Assets/ModelAsset.hpp>

namespace Hyper
{
    class HYPER_API HouseDoorEntity : public Entity, public IActionProvider
    {
    private:
        JPH::Body* m_Body = nullptr;
        bool m_Open = false;
        float m_OpenProgress = 0;

    public:
        HouseDoorEntity();
        ~HouseDoorEntity() override = default;

        EntityMobility GetMobility() override;
        void OnFixedUpdate(FixedUpdateEvent& event) override;
        void OnUpdate(UpdateEvent& event) override;
        void OnDraw(DrawCallCollectContext& context) override;

        std::vector<Action> Provide() override;
    };
}
