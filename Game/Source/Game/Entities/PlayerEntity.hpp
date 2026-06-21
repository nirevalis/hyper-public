#pragma once

#include <Core/Common.hpp>
#include <Scene/Entity.hpp>
#include <Renderer/Mesh.hpp>
#include <Jolt/Physics/Character/CharacterVirtual.h>

namespace Hyper
{
	class LocalPlayer;

	class HYPER_API PlayerEntity : public Entity
	{
	private:
		friend class LocalPlayer;
		friend class Server;

		JPH::Quat m_Rotation = JPH::Quat::sIdentity();
		JPH::Vec3 m_Displacement = JPH::Vec3::sZero();
		JPH::Vec3 m_DesiredVelocity = JPH::Vec3::sZero();

		float m_JumpPower = 0.0f;
		float m_OldJumpPower = 0.0f;
		bool m_HasGravity = true;
		bool m_ControlMovementInAir = false;
		bool m_ControlRotationInAir = false;
		bool m_AllowSliding = false;
		bool m_AlreadyJumped = false;

		JPH::CharacterVirtual* m_CharacterController = nullptr;
		JPH::Ref<JPH::Shape> m_Shape = nullptr;

		Float3 getMoveDirection();
		void updatePhysics();
		void processInput(double deltaTime);
		void updateCamera(double deltaTime);
	public:
		PlayerEntity();
		~PlayerEntity() override = default;

		LocalPlayer* LocalPlayer = nullptr;

		void Move(const Float3& displacement);

		void SetTranslation(const Float3& position);
		void SetOrientation(const Quaternion& quaternion);

		EntityMobility GetMobility() override;
		void OnFixedUpdate(FixedUpdateEvent& event) override;
		void OnUpdate(UpdateEvent& event) override;
		void OnDraw(DrawCallCollectContext& context) override;
	};
}