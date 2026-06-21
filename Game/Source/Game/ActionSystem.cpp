#include "ActionSystem.hpp"

#include <ranges>
#include <GLFW/glfw3.h>
#include <Physics/JoltMath.hpp>
#include <Scene/Scene.hpp>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>

#include "Game.hpp"

namespace Hyper
{
    ActionSystem::ActionSystem()
    {
        Game::Get().GetGameWindow()->OnKeyPress.Connect(std::bind(&ActionSystem::onKeyPress, this, std::placeholders::_1));
    }

    void ActionSystem::onKeyPress(int32 key)
    {
        if (m_KeyToAction.contains(key))
            m_KeyToAction[key]->Callback();
    }

    void ActionSystem::Update(Scene *scene)
    {
        m_Actions.clear();
        m_KeyToAction.clear();

        JPH::Vec3 rayStart = JoltMath::FromFloat3(scene->Camera.GetTransform().Position);
        JPH::Vec3 rayDirection = JoltMath::FromFloat3(scene->Camera.GetTransform().GetForward());
        float maxDistance = 5.0f;

        JPH::RRayCast ray(rayStart, rayDirection * maxDistance);

        JPH::RayCastResult hit;
        bool hasHit = scene->PhysicsScene.System.GetNarrowPhaseQuery().CastRay(ray, hit);

        if (hasHit)
        {
            JPH::Vec3 hitPoint = ray.GetPointOnRay(hit.mFraction);
            JPH::BodyID hitBodyID = hit.mBodyID;

            auto* storage = reinterpret_cast<BodyDataStorage *>(scene->PhysicsScene.System.GetBodyInterface().GetUserData(hitBodyID));

            if (storage)
                if (auto* provider = storage->As<IActionProvider>())
                    m_Actions[provider->ObjectName] = provider->Provide();

            for (auto& list : m_Actions | std::ranges::views::values)
            {
                for (int32 i =0 ;i < list.size();i++)
                {
                    Action* action = &list[i];
                    m_KeyToAction[action->Keybind] = action;
                }
            }
        }
    }

    std::unordered_map<String, std::vector<Action>>& ActionSystem::GetActions()
    {
        return m_Actions;
    }
}
