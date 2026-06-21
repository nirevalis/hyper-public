#pragma once

#include <Core/Common.hpp>
#include <Renderer/DrawContext.hpp>

namespace Hyper
{
	enum class EventType
	{
		SimulationUpdate,
		Update,
		SimulationFixedUpdate,
		FixedUpdate,
		SceneDraw
	};

	class HYPER_API Event
	{
	public:
		Event() = default;
		virtual ~Event() = default;

		virtual EventType GetEventType() = 0;
	};

	class HYPER_API SimulationUpdateEvent : public Event
	{
	public:
		double DeltaTime = 0;

		EventType GetEventType() override
		{
			return EventType::SimulationUpdate;
		}
	};

	class HYPER_API UpdateEvent : public Event
	{
	public:
		double DeltaTime = 0;

		EventType GetEventType() override
		{
			return EventType::Update;
		}
	};

	class HYPER_API SimulationFixedUpdateEvent : public Event
	{
	public:
		EventType GetEventType() override
		{
			return EventType::SimulationFixedUpdate;
		}
	};

	class HYPER_API FixedUpdateEvent : public Event
	{
	public:
		EventType GetEventType() override
		{
			return EventType::FixedUpdate;
		}
	};

	class HYPER_API SceneDrawEvent : public Event
	{
	public:
		DrawCallCollectContext& Context;

		SceneDrawEvent(DrawCallCollectContext& context) : Context(context)
		{

		}

		EventType GetEventType() override
		{
			return EventType::SceneDraw;
		}
	};
}