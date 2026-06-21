#include "BulletEntity.hpp"

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
	BulletEntity::BulletEntity()
	{
		GetTransform().Scale = Float3::One * 0.1f;
	}

	EntityMobility BulletEntity::GetMobility()
	{
		return EntityMobility::DynamicTransform;
	}

	void BulletEntity::OnFixedUpdate(FixedUpdateEvent& event)
	{
		if (m_Body == nullptr)
		{
			auto& bodyInterface = m_Scene->PhysicsScene.GetBodyInterface();
			JPH::Ref<JPH::Shape> shape = PhysicsScene::CreateConvexHull( AssetRegistry::GetAs<ModelAsset>("Models/Sphere")->GetModelData()->Meshes);

			JPH::ScaledShape* scaled = new JPH::ScaledShape(shape, JoltMath::FromFloat3(GetTransform().Scale));

			JPH::BodyCreationSettings settings(scaled, JoltMath::FromFloat3(GetTransform().Position), JoltMath::FromQuaternion(GetTransform().Orientation), JPH::EMotionType::Dynamic, Layers::MOVING);
			m_Body = bodyInterface.CreateBody(settings);
			bodyInterface.AddBody(m_Body->GetID(), JPH::EActivation::Activate);
		}

		GetTransform().Position = JoltMath::ToFloat3(m_Body->GetPosition());
		GetTransform().Orientation = JoltMath::ToQuaternion(m_Body->GetRotation());
	}

	void BulletEntity::OnUpdate(UpdateEvent& event)
	{
		if (m_Body && Velocity != Float3::Zero)
		{
			m_Body->AddForce(JoltMath::FromFloat3(Velocity));
			Velocity = Float3::Zero;
		}
	}

	void BulletEntity::OnDraw(DrawCallCollectContext& context)
	{
		ModelFlushedData* data = AssetRegistry::GetAs<ModelAsset>("Models/Sphere")->GetModelData();

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
