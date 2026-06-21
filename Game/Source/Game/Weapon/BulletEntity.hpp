#pragma once

#include <Core/Common.hpp>
#include <Game/ActionSystem.hpp>
#include <Scene/Entity.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>
#include <Renderer/Assets/ModelAsset.hpp>

namespace Hyper
{
    class HYPER_API BulletEntity : public Entity
    {
    private:
        JPH::Body* m_Body = nullptr;

    public:
        Float3 Velocity;

        BulletEntity();
        ~BulletEntity() override = default;

        EntityMobility GetMobility() override;
        void OnFixedUpdate(FixedUpdateEvent& event) override;
        void OnUpdate(UpdateEvent& event) override;
        void OnDraw(DrawCallCollectContext& context) override;
    };
}
