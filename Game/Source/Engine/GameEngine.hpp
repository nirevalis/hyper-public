#pragma once

#include <Core/Common.hpp>
#include "LayerStack.hpp"
#include <Renderer/Device.hpp>
#include <queue>
#include <mutex>

namespace Hyper
{
	class HYPER_API GameEngine
	{
	private:
		inline static GameEngine* m_Instance;

		std::queue<Event*> m_EventQueue;
		std::mutex m_EventQueueMutex;

		LayerStack m_LayerStack;
		uint64 m_TickCount = 0;
		DeviceHelper m_DeviceHelper;
		bool m_ShouldExit = false;

	public:
		GameEngine();
		~GameEngine();

		void Start();
		void RequestExit();

		void QueueEvent(Event* event);

		uint64 GetTick() const;

		LayerStack& GetLayerStack();
		DeviceHelper& GetDeviceHelper();

		static GameEngine& Get()
		{
			return *m_Instance;
		}
	};
}