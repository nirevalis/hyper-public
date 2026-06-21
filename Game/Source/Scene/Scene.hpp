#pragma once

#include <Core/Common.hpp>
#include <Core/GUID.hpp>
#include <Engine/Event.hpp>
#include "Entity.hpp"
#include <unordered_map>
#include "Renderer/Camera.hpp"
#include <Physics/PhysicsScene.hpp>

namespace Hyper
{
	class HYPER_API Scene
	{
	private:
		std::unordered_map<Guid, Entity*> m_Entities;

	public:
		Camera Camera;
		PhysicsScene PhysicsScene;

		Scene() = default;
		~Scene() = default;

		void SetGUID(Entity* entity, const Guid& guid);
		void Register(Entity* entity);
		Entity* Find(const Guid& guid);
		void OnEvent(Hyper::Event& event);
	};
}
