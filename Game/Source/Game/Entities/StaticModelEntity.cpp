#include "StaticModelEntity.hpp"

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
	StaticModelEntity::StaticModelEntity(ModelAsset *model)
		: m_ModelAsset(model)
	{
		ObjectName = "EX_OBJECT_INTERACT";
	}

	EntityMobility StaticModelEntity::GetMobility()
	{
		return EntityMobility::DynamicTransform;
	}

	void StaticModelEntity::OnFixedUpdate(FixedUpdateEvent& event)
	{
		if (m_Body == nullptr)
		{
			auto& bodyInterface = m_Scene->PhysicsScene.GetBodyInterface();
			JPH::Ref<JPH::Shape> shape = PhysicsScene::CreateConvexHull(m_ModelAsset->GetModelData()->Meshes);

			JPH::ScaledShape* scaled = new JPH::ScaledShape(shape, JoltMath::FromFloat3(GetTransform().Scale));

			JPH::BodyCreationSettings settings(scaled, JoltMath::FromFloat3(GetTransform().Position), JoltMath::FromQuaternion(GetTransform().Orientation), PhysicsBodyMode, PhysicsLayer);
			settings.mAllowDynamicOrKinematic = true;
			m_Body = bodyInterface.CreateBody(settings);
			bodyInterface.AddBody(m_Body->GetID(), JPH::EActivation::Activate);

			bodyInterface.SetUserData(m_Body->GetID(), reinterpret_cast<uint64>(BodyDataStorage::CreateActionProvider(this)));
		}

		GetTransform().Position = JoltMath::ToFloat3(m_Body->GetPosition());
		GetTransform().Orientation = JoltMath::ToQuaternion(m_Body->GetRotation());
	}

	std::vector<Action> StaticModelEntity::Provide()
	{
		std::vector<Action> actions;

		actions.emplace_back(GLFW_KEY_E, "Apply gravity", [this]()
		{
			m_Scene->PhysicsScene.GetBodyInterface().SetObjectLayer(m_Body->GetID(), Layers::MOVING);
			m_Scene->PhysicsScene.GetBodyInterface().SetMotionType(m_Body->GetID(), JPH::EMotionType::Dynamic, JPH::EActivation::Activate);
		});

		return actions;
	}

	void StaticModelEntity::OnUpdate(UpdateEvent& event)
	{

	}

	void StaticModelEntity::OnDraw(DrawCallCollectContext& context)
	{
		ModelFlushedData* data = m_ModelAsset->GetModelData();

		for (Mesh& mesh : data->Meshes)
		{
			DrawCall call = {};
			call.Mesh = &mesh;
			call.Material = mesh.Material;
			call.Transform = GetTransform();

			context.Add(call);
		}
	}
}
