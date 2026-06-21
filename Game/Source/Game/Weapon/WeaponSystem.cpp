#include "WeaponSystem.hpp"

#include <Game/Game.hpp>
#include <Game/GameWindow.hpp>
#include <Physics/JoltMath.hpp>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>

#include "BulletEntity.hpp"

namespace Hyper
{
    void WeaponSystem::Update(double deltaTime)
    {
        GameInput& input = Game::Get().GetGameInput();
        Scene* scene = Game::Get().GetLocalPlayer()->Scene;
        PlayerEntity* player = Game::Get().GetLocalPlayer()->Player;

        if (input.IsMouseDown(GLFW_MOUSE_BUTTON_LEFT) && m_TimeAccumulation <= 0)
        {
            m_TimeAccumulation+=0.25f;

            JPH::Vec3 rayStart = JoltMath::FromFloat3(scene->Camera.GetTransform().Position);
            JPH::Vec3 rayDirection = JoltMath::FromFloat3(scene->Camera.GetTransform().GetForward());
            float maxDistance = 100.0f;

            JPH::RRayCast ray(rayStart, rayDirection * maxDistance);

            JPH::RayCastResult hit;
            bool hasHit = scene->PhysicsScene.System.GetNarrowPhaseQuery().CastRay(ray, hit);

            if (hasHit)
            {
                JPH::Vec3 hitPoint = ray.GetPointOnRay(hit.mFraction);
                JPH::BodyID hitBodyID = hit.mBodyID;

                JPH::Vec3 bulletDir = (hitPoint - rayStart).Normalized();
                float impulseStrength = 8000.0f;
                JPH::Vec3 impulse = bulletDir * impulseStrength;

                scene->PhysicsScene.System.GetBodyInterface().AddImpulse(hitBodyID, impulse);

                JPH::Vec3 centerOfMass = scene->PhysicsScene.System.GetBodyInterface().GetCenterOfMassPosition(hitBodyID);
                JPH::Vec3 r = hitPoint - centerOfMass;

                JPH::Vec3 angularImpulse = r.Cross(impulse);

                 scene->PhysicsScene.System.GetBodyInterface().AddAngularImpulse(hitBodyID, angularImpulse);
            }
        }

        if (m_TimeAccumulation > 0)
            m_TimeAccumulation -= deltaTime;
    }

}
