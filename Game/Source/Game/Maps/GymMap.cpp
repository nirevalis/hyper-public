#include "GymMap.hpp"

#include <Asset/AssetRegistry.hpp>
#include <Game/Entities/HouseDoorEntity.hpp>
#include <Game/Entities/StaticModelEntity.hpp>

namespace Hyper
{
    namespace Internal
    {
        static Scene* Scene;

        static StaticModelEntity* CreateStaticMapObject(const String& asset, const Float3& position = Float3::Zero, const Quaternion& rotation = Quaternion::Euler(Float3::Zero), const Float3& scale = Float3::One)
        {
            StaticModelEntity* model = new StaticModelEntity(AssetRegistry::GetAs<ModelAsset>(asset));
            model->PhysicsLayer = Layers::NON_MOVING;
            model->PhysicsBodyMode = JPH::EMotionType::Static;

            model->GetTransform().Position = position;
            model->GetTransform().Orientation = rotation;
            model->GetTransform().Scale = scale;

            Scene->Register(model);

            return model;
        }

        static StaticModelEntity* CreateDynamicMapObject(const String& asset, const Float3& position = Float3::Zero, const Quaternion& rotation = Quaternion::Euler(Float3::Zero), const Float3& scale = Float3::One)
        {
            StaticModelEntity* model = new StaticModelEntity(AssetRegistry::GetAs<ModelAsset>(asset));
            model->PhysicsLayer = Layers::MOVING;
            model->PhysicsBodyMode = JPH::EMotionType::Dynamic;

            model->GetTransform().Position = position;
            model->GetTransform().Orientation = rotation;
            model->GetTransform().Scale = scale;

            Scene->Register(model);

            return model;
        }

        template <typename T>
        static T* CreateCustomMapObject(const Float3& position = Float3::Zero, const Quaternion& rotation = Quaternion::Euler(Float3::Zero), const Float3& scale = Float3::One)
        {
            T* model = new T();

            model->GetTransform().Position = position;
            model->GetTransform().Orientation = rotation;
            model->GetTransform().Scale = scale;

            Scene->Register(model);

            return model;
        }
    }

    void GymMap::Load(Scene* scene)
    {
        Internal::Scene = scene;

        //Terrain
        Internal::CreateStaticMapObject("Models/Cube", Float3::Zero, Quaternion::Euler(Float3(0,0,0)), Float3{20,0.05f,20});

        //Stairs
        Internal::CreateStaticMapObject("Models/Stairs", Float3{0,2.42729f,0}, Quaternion::Euler(Float3(0,180,0)), Float3::One * 0.001f);
        Internal::CreateStaticMapObject("Models/Stairs", Float3{-12.4716f,2.42729f,5.04031f}, Quaternion::Euler(Float3(0,-90, 0)), Float3::One * 0.001f);

        //Footbridge
        Internal::CreateStaticMapObject("Models/Cube", Float3{-4.04691f,4.80486f, 5.0797f}, Quaternion::Euler(Float3(0,0,0)), Float3{5.16f, 0.05f, 1.82f});

        Internal::CreateDynamicMapObject("Models/Props/Mailbox", Float3{-2,0,-2}, Quaternion::Euler(Float3(0,0,0)), Float3::One* 4);

        Internal::CreateCustomMapObject<HouseDoorEntity>(Float3{3,0,3});
        //Internal::CreateStaticMapObject("Models/Doors/HouseDoor", Float3{0,0,2}, Quaternion::Euler(Float3(0,0,0)), Float3{1,1,1});
    }
}
