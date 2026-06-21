#include "HouseDoorEntity.hpp"

#include <Game/Game.hpp>
#include <Game/LocalPlayer.hpp>
#include "Asset/AssetRegistry.hpp"
#include "Core/Math/Color.hpp"
#include "Engine/GameEngine.hpp"
#include "Renderer/Assets/ModelAsset.hpp"

#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>
#include <Physics/JoltMath.hpp>

namespace Hyper
{
	HouseDoorEntity::HouseDoorEntity()
	{
		ObjectName = "House Door";
	}

	EntityMobility HouseDoorEntity::GetMobility()
	{
		return EntityMobility::DynamicTransform;
	}

	void HouseDoorEntity::OnFixedUpdate(FixedUpdateEvent& event)
	{
		if (m_Body == nullptr)
		{
			auto& bodyInterface = m_Scene->PhysicsScene.GetBodyInterface();
			JPH::Ref<JPH::Shape> shape = PhysicsScene::CreateConvexHull(AssetRegistry::GetAs<ModelAsset>("Models/Doors/House/Frame")->GetModelData()->Meshes);

			JPH::BodyCreationSettings settings(shape, JoltMath::FromFloat3(GetTransform().Position), JoltMath::FromQuaternion(GetTransform().Orientation), JPH::EMotionType::Static, Layers::NON_MOVING);
			m_Body = bodyInterface.CreateBody(settings);

			bodyInterface.AddBody(m_Body->GetID(), JPH::EActivation::Activate);

			bodyInterface.SetUserData(m_Body->GetID(), reinterpret_cast<uint64>(BodyDataStorage::CreateActionProvider(this)));
		}

		GetTransform().Position = JoltMath::ToFloat3(m_Body->GetPosition());
		GetTransform().Orientation = JoltMath::ToQuaternion(m_Body->GetRotation());
	}

	std::vector<Action> HouseDoorEntity::Provide()
	{
		std::vector<Action> actions;

		actions.emplace_back(GLFW_KEY_E, m_Open ? "Close" : "Open", [this]()
		{
			m_Open = !m_Open;
			m_Scene->PhysicsScene.GetBodyInterface().SetIsSensor(m_Body->GetID(), m_Open);
		});

		return actions;
	}

	void HouseDoorEntity::OnUpdate(UpdateEvent& event)
	{
		if (m_Open && m_OpenProgress <= 90)
			m_OpenProgress += event.DeltaTime * 90;
		else if (m_Open)
			m_OpenProgress = 90;

		if (!m_Open &&  m_OpenProgress > 0)
			m_OpenProgress -= event.DeltaTime * 90;
	}

	void HouseDoorEntity::OnDraw(DrawCallCollectContext& context)
	{
		ModelFlushedData* door = AssetRegistry::GetAs<ModelAsset>("Models/Doors/House/Door")->GetModelData();
		ModelFlushedData* frame = AssetRegistry::GetAs<ModelAsset>("Models/Doors/House/Frame")->GetModelData();

		//Draw door
		{
			RadiansTransform transform = GetTransform();
			transform.Position.X += 0.65f;
			transform.Position.Y += 1.47f;
			transform.Orientation = Quaternion::Euler(Float3(0,m_OpenProgress,0));

			{
				DrawCall call = {};
				call.Mesh = &door->Meshes[0];
				call.Material = door->Meshes[0].Material;
				call.Transform = transform;

				context.Add(call);
			}

			{
				DrawCall call = {};
				call.Mesh = &door->Meshes[1];
				call.Material = door->Meshes[1].Material;
				call.Transform = transform;

				context.Add(call);
			}
		}

		//Draw frame
		{
			DrawCall call = {};
			call.Mesh = &frame->Meshes[0];
			call.Material = frame->Meshes[0].Material;
			call.Transform = GetTransform();

			context.Add(call);
		}
	}
}
