#include "FallingEntity.hpp"

#include <Game/Game.hpp>
#include <Game/LocalPlayer.hpp>
#include <Jolt/Physics/Collision/PhysicsMaterialSimple.h>

#include "Asset/AssetRegistry.hpp"
#include "Core/Math/Color.hpp"
#include "Engine/GameEngine.hpp"
#include "Renderer/Assets/ModelAsset.hpp"
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Physics/JoltMath.hpp>

namespace Hyper
{
	FallingEntity::FallingEntity()
	{
		GetTransform().Position = Float3(0,3,0);
	}

	EntityMobility FallingEntity::GetMobility()
	{
		return EntityMobility::DynamicTransform;
	}

	void FallingEntity::OnFixedUpdate(FixedUpdateEvent& event)
	{
		if (m_Body == nullptr)
		{
			srand(time(NULL));
			auto& bodyInterface = m_Scene->PhysicsScene.GetBodyInterface();

			JPH::Shape* shape = AssetRegistry::GetAs<ModelAsset>("Models/Sphere")->GetModelData()->Meshes[0].ConvexHullShape.Create().Get();
			JPH::BodyCreationSettings settings(shape, JoltMath::FromFloat3(GetTransform().Position), JoltMath::FromQuaternion(GetTransform().Orientation), JPH::EMotionType::Dynamic, Layers::MOVING);

			JPH::MassProperties msp;
			msp.ScaleToMass(0.1f);

			settings.mMassPropertiesOverride = msp;
			settings.mFriction = 1.0f;
			settings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;

			m_Body = bodyInterface.CreateBody(settings);
			bodyInterface.AddBody(m_Body->GetID(), JPH::EActivation::Activate);
		}

		GetTransform().Position = JoltMath::ToFloat3(m_Body->GetPosition());
		GetTransform().Orientation = JoltMath::ToQuaternion(m_Body->GetRotation());
	}

	void FallingEntity::OnUpdate(UpdateEvent& event)
	{

	}

	static void RenderModelEx(const RadiansTransform& transform, DrawCallCollectContext& context, ModelAsset* model)
	{
		ModelFlushedData* data = model->GetModelData();

		for (Mesh& mesh : data->Meshes)
		{
			Material mat{};
			mat.Type = SurfaceType::Solid;
			mat.Color = MaterialValue<Float4>::FromValue(Color::FromRGB(255,255,255).ToVector());

			DrawCall call = {};
			call.Mesh = &mesh;
			call.Material = mat;
			call.Transform = transform;

			context.Add(call);
		}
	}

	void FallingEntity::OnDraw(DrawCallCollectContext& context)
	{
		RenderModelEx(GetTransform(), context, AssetRegistry::GetAs<ModelAsset>("Models/Sphere"));
	}
}
