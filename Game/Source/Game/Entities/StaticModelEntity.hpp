#pragma once

#include <Core/Common.hpp>
#include <Game/ActionSystem.hpp>
#include <Scene/Entity.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>
#include <Renderer/Assets/ModelAsset.hpp>

namespace Hyper
{
    class HYPER_API StaticModelEntity : public Entity, public IActionProvider
    {
    private:
        JPH::Body* m_Body = nullptr;
        ModelAsset* m_ModelAsset = nullptr;

    public:
        JPH::EMotionType PhysicsBodyMode;
        uint16 PhysicsLayer;

        StaticModelEntity(ModelAsset* model);
        ~StaticModelEntity() override = default;

        EntityMobility GetMobility() override;
        void OnFixedUpdate(FixedUpdateEvent& event) override;
        void OnUpdate(UpdateEvent& event) override;
        void OnDraw(DrawCallCollectContext& context) override;

        std::vector<Action> Provide() override;
    };
}
