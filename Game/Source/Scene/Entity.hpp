#pragma once

#include <Core/Common.hpp>
#include <Core/GUID.hpp>
#include <Core/Math/Transform.hpp>
#include <Engine/Event.hpp>
#include <Renderer/DrawContext.hpp>

namespace Hyper
{
	enum class HYPER_API EntityMobility
	{
		Static,
		DynamicTransform,
		Deformable
	};

	class HYPER_API Entity
	{
	private:
		friend class Scene;

		Guid m_Id;
		RadiansTransform m_Transform;

	protected:
		Scene* m_Scene;

	public:
		Entity() : m_Id(Guid::Random())
		{

		};

		virtual ~Entity() = default;

	public:
		Guid GetId();
		RadiansTransform& GetTransform();

		virtual EntityMobility GetMobility();
	public:
		virtual void OnUpdate(UpdateEvent& event)
		{

		}

		virtual void OnFixedUpdate(FixedUpdateEvent& event)
		{

		}

		virtual void OnSimulationUpdate(SimulationUpdateEvent& event)
		{

		}

		virtual void OnSimulationFixedUpdate(SimulationFixedUpdateEvent& event)
		{

		}

		virtual void OnDraw(DrawCallCollectContext& context)
		{

		}
	};
}