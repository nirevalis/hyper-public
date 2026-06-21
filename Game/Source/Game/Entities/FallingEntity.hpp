#pragma once

#include <Core/Common.hpp>
#include <Scene/Entity.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>

namespace Hyper
{
    class HYPER_API FallingEntity : public Entity
    {
    private:
        JPH::Body* m_Body = nullptr;
    public:
        FallingEntity();
        ~FallingEntity() override = default;

        EntityMobility GetMobility() override;
        void OnFixedUpdate(FixedUpdateEvent& event) override;
        void OnUpdate(UpdateEvent& event) override;
        void OnDraw(DrawCallCollectContext& context) override;
    };
}