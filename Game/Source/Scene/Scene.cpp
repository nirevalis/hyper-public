#include "Scene.hpp"
#include <ranges>

namespace Hyper
{
	void Scene::SetGUID(Entity* entity, const Guid& guid)
	{
		if (m_Entities.contains(guid))
			m_Entities.erase(m_Entities.find(guid), m_Entities.end());

		entity->m_Id = guid;
	}

	void Scene::Register(Entity* entity)
	{
		entity->m_Scene = this;
		m_Entities.emplace(entity->GetId(), entity);
	}

	Entity* Scene::Find(const Guid& guid)
	{
		return m_Entities.at(guid);
	}

	void Scene::OnEvent(Hyper::Event& event)
	{
		switch (event.GetEventType())
		{
		case EventType::Update:
			for (Entity* entity : m_Entities | std::views::values)
			{
				entity->OnUpdate(dynamic_cast<Hyper::UpdateEvent&>(event));
			}
			break;
		case EventType::FixedUpdate:
			PhysicsScene.Simulate(1.0 / 128.0);

			for (Entity* entity : m_Entities | std::views::values)
			{
				entity->OnFixedUpdate(dynamic_cast<Hyper::FixedUpdateEvent&>(event));
			}
			break;
		case EventType::SceneDraw:
		{
			auto& drawEvent = static_cast< SceneDrawEvent& >(event);
			for (Entity* entity : m_Entities | std::views::values)
			{
				entity->OnDraw(drawEvent.Context);
			}
			break;
		}
		}
	}
}