#include "PlayerEntity.hpp"

#include <Game/Game.hpp>
#include <Game/LocalPlayer.hpp>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Math/Math.h>
#include <Jolt/Math/Quat.h>
#include <Jolt/Math/Vec3.h>
#include <Physics/JoltMath.hpp>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>

#include "Asset/AssetRegistry.hpp"
#include "Core/Math/Color.hpp"
#include "Engine/GameEngine.hpp"
#include "Renderer/Assets/ModelAsset.hpp"

namespace Hyper
{
	Material mat;

	PlayerEntity::PlayerEntity()
	{
		GetTransform().Position = Float3{0,10,0};
		GetTransform().Scale = Float3{0.75f,2.25f,0.75f};
	}

	EntityMobility PlayerEntity::GetMobility()
	{
		return EntityMobility::DynamicTransform;
	}

	void PlayerEntity::processInput(double deltaTime)
	{
		GameInput& input = Game::Get().GetGameInput();
		RadiansTransform& transform = GetTransform();

			//Update player position
			Float3 moveDir = Float3::Zero;

			if (input.IsKeyDown(GLFW_KEY_W)) moveDir = moveDir + transform.GetForward();
			if (input.IsKeyDown(GLFW_KEY_S)) moveDir = moveDir - transform.GetForward();
			if (input.IsKeyDown(GLFW_KEY_A)) moveDir = moveDir - transform.GetRight();
			if (input.IsKeyDown(GLFW_KEY_D)) moveDir = moveDir + transform.GetRight();

			if (moveDir.GetMagnitude() > 0.0f)
				moveDir.Normalize();
			else moveDir = Float3::Zero;

			Move(
				moveDir * 4 * deltaTime
				);

		m_JumpPower = m_CharacterController && m_CharacterController->IsSupported() && input.IsKeyDown(GLFW_KEY_SPACE) ? 3 : 0;
	}

	void PlayerEntity::updateCamera(double deltaTime)
	{
		GameInput& input = Game::Get().GetGameInput();
		RadiansTransform& transform = GetTransform();
		GameInput::MouseData data = input.GetMouseData();
		Scene* scene = Game::Get().GetLocalPlayer()->Scene;

		Float2 mouseDelta = Float2(
		data.MousePosition.X - data.LastMousePosition.X,
		data.LastMousePosition.Y - data.MousePosition.Y
		) * 500 * deltaTime;

		auto& camera = scene->Camera.GetTransform();
		camera.Position = GetTransform().Position + Float3{0,1.6875f,0};

		camera.Orientation.X += mouseDelta.Y;
		camera.Orientation.Y += mouseDelta.X;

		camera.Orientation.X = Math::Clamp(camera.Orientation.X, -89.0f, 89.0f);
		SetOrientation(Quaternion::Euler(Float3(0,camera.Orientation.Y,0)));
	}


	void PlayerEntity::Move(const Float3 &displacement)
	{
		m_Displacement = m_Displacement + JoltMath::FromFloat3(displacement);
	}

	void PlayerEntity::SetOrientation(const Quaternion &quaternion)
	{
		if (m_CharacterController)
			m_CharacterController->SetRotation(JoltMath::FromQuaternion(quaternion));
	}

	void PlayerEntity::SetTranslation(const Float3 &position)
	{
		if (m_CharacterController)
			m_CharacterController->SetPosition(JoltMath::FromFloat3(position));
	}

	void PlayerEntity::updatePhysics()
	{
	    JPH::PhysicsSystem& system = m_Scene->PhysicsScene.System;

	    if (m_CharacterController == nullptr)
	    {
	        auto& bodyInterface = m_Scene->PhysicsScene.GetBodyInterface();

	        JPH::Ref<JPH::CharacterVirtualSettings> settings = new JPH::CharacterVirtualSettings();

	        settings->mMaxSlopeAngle = JPH::DegreesToRadians(45.0f);

	        m_Shape = PhysicsScene::CreateConvexHull({AssetRegistry::GetAs<ModelAsset>("Models/Cylinder")->GetModelData()->Meshes[0]});

	        settings->mShape = new JPH::ScaledShape(m_Shape, JoltMath::FromFloat3(GetTransform().Scale));

	        settings->mMaxStrength = 100.0f;
	        settings->mBackFaceMode = JPH::EBackFaceMode::CollideWithBackFaces;
	        settings->mCharacterPadding = 0.02f;
	        settings->mPredictiveContactDistance = 0.1f;

	        m_CharacterController = new JPH::CharacterVirtual(settings, JoltMath::FromFloat3(GetTransform().Position), JoltMath::FromQuaternion(GetTransform().Orientation), &m_Scene->PhysicsScene.System);
	        m_HasGravity = false;
	        m_ControlMovementInAir = true;
	        m_ControlRotationInAir = true;
	    }

	    m_DesiredVelocity = m_Displacement / (1/128.0f);
	    m_CharacterController->SetRotation((m_CharacterController->GetRotation() * m_Rotation).Normalized());
	    m_AllowSliding = !m_CharacterController->IsSupported() || !m_Displacement.IsNearZero();

	    m_CharacterController->UpdateGroundVelocity();

	    JPH::Vec3 currentVerticalVelocity = JPH::Vec3(0, m_CharacterController->GetLinearVelocity().GetY(), 0);
	    JPH::Vec3 groundVelocity = m_CharacterController->GetGroundVelocity();

	    bool jumping = (currentVerticalVelocity.GetY() - groundVelocity.GetY()) >= 0.1f;

	    JPH::Vec3 newVelocity;
	    if (m_CharacterController->GetGroundState() == JPH::CharacterVirtual::EGroundState::OnGround &&
	        (!m_CharacterController->IsSlopeTooSteep(m_CharacterController->GetGroundNormal())))
	    {
	        newVelocity = groundVelocity;

	        if (m_JumpPower > 0.0f && !jumping)
	        {
	            newVelocity += JPH::Vec3(0, m_JumpPower, 0);
	            m_OldJumpPower = m_JumpPower;
	            m_JumpPower = 0.0f;
	        }
	    }
	    else
	    {
	        newVelocity = currentVerticalVelocity;
	    }

	    newVelocity += JoltMath::FromFloat3(Float3{0, -9.81f, 0} * 1/128.0f);

	    if (m_CharacterController->IsSupported() || m_ControlMovementInAir)
	    {
	        newVelocity += m_DesiredVelocity;
	    }
	    else
	    {
	        JPH::Vec3 currentHorizontalVelocity = m_CharacterController->GetLinearVelocity() - currentVerticalVelocity;
	        newVelocity += currentHorizontalVelocity;
	    }

	    m_CharacterController->SetLinearVelocity(newVelocity);

	    JPH::Vec3 gravity = JPH::Vec3{0, -9.81f, 0};

	    auto broadPhaseLayerFilter = system.GetDefaultBroadPhaseLayerFilter(JPH::ObjectLayer(Layers::MOVING));
	    auto layerFilter = system.GetDefaultLayerFilter(JPH::ObjectLayer(Layers::MOVING));
	    auto* tempAllocator = &m_Scene->PhysicsScene.TempAllocator;

	    JPH::CharacterVirtual::ExtendedUpdateSettings updateSettings;
	    updateSettings.mStickToFloorStepDown = JPH::Vec3(0, -0.5f, 0);
	    updateSettings.mWalkStairsStepUp = JPH::Vec3::sZero();
	    updateSettings.mWalkStairsStepForwardTest = 0.0f;
	    updateSettings.mWalkStairsStepDownExtra = JPH::Vec3::sZero();

	    m_CharacterController->ExtendedUpdate(1/128.0f, gravity, updateSettings, broadPhaseLayerFilter, layerFilter, {}, {}, *tempAllocator);

	    if (m_CharacterController->IsSupported() || m_ControlMovementInAir)
	    {
	        m_Displacement = JPH::Vec3::sZero();
	    }
	    if (m_CharacterController->IsSupported() || m_ControlRotationInAir)
	    {
	        m_Rotation = JPH::Quat::sIdentity();
	    }

	    GetTransform().Position = JoltMath::ToFloat3(m_CharacterController->GetPosition());
	    GetTransform().Orientation = JoltMath::ToQuaternion(m_CharacterController->GetRotation());
	}

	void PlayerEntity::OnFixedUpdate(FixedUpdateEvent& event)
	{
		processInput(1.0f/128);
		updatePhysics();
	}

	void PlayerEntity::OnUpdate(UpdateEvent& event)
	{
		updateCamera(event.DeltaTime);
	}

	static void RenderModel(const RadiansTransform& transform, DrawCallCollectContext& context, ModelAsset* model)
	{
		ModelFlushedData* data = model->GetModelData();

		for (Mesh& mesh : data->Meshes)
		{
			DrawCall call = {};
			call.Mesh = &mesh;
			call.Material = mat;
			call.Transform = transform;

			context.Add(call);
		}
	}

	void PlayerEntity::OnDraw(DrawCallCollectContext& context)
	{

	}
}
