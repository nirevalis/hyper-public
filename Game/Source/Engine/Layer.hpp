#pragma once

#include "Event.hpp"

namespace Hyper
{
	class HYPER_API ILayer
	{
	public:
		ILayer() = default;
		virtual ~ILayer() = default;

		virtual void OnStart() = 0;
		virtual void OnExit() = 0;
		virtual void OnEvent(Event& event) = 0;
	};
}