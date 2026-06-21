#include "GameEngine.hpp"
#include <chrono>
#include <spdlog/spdlog.h>
#include <Core/GUID.hpp>
#include <Threading/JobManager.hpp>

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>

namespace Hyper
{
	using namespace std::chrono;

	class HYPER_API Time
	{
	private:
		steady_clock::time_point m_LastTime = high_resolution_clock::now();
		double m_FpsTimer = 0;
		double m_DeltaTime = 0;
		int32 m_Frames = 0;
		int32 m_FPS = 0;
		double m_Accumulator = 0.0;

	public:
		inline int32 GetFPS() const
		{
			return m_FPS;
		}

		inline double GetDeltaTime() const
		{
			return m_DeltaTime;
		}

		FORCE_INLINE void Update()
		{
			auto currentTime = high_resolution_clock::now();
			m_DeltaTime = duration<double>(currentTime - m_LastTime).count();
			m_LastTime = currentTime;

			m_FpsTimer += m_DeltaTime;
			m_Frames++;

			if (m_FpsTimer >= 1.0)
			{
				m_FPS = m_Frames;
				m_FpsTimer = 0.0;
				m_Frames = 0;
			}
		}

		FORCE_INLINE bool ShouldCallFixed()
		{
			bool shouldCall = false;

			constexpr double fixedDeltaTime = 1.0 / 128.0;
			m_Accumulator += m_DeltaTime;

			while (m_Accumulator >= fixedDeltaTime)
			{
				shouldCall = true;
				m_Accumulator -= fixedDeltaTime;
			}

			return shouldCall;
		}
	};

	void LayerStackEmitter::EmitStart(Hyper::LayerStack& stack)
	{
		for (ILayer* layer : stack) layer->OnStart();
	}

	void LayerStackEmitter::EmitExit(Hyper::LayerStack& stack)
	{
		for (ILayer* layer : stack) layer->OnExit();
	}

	void LayerStackEmitter::EmitEvent(Hyper::LayerStack& stack, Hyper::Event& event)
	{
		for (ILayer* layer : stack) layer->OnEvent(event);
	}

	GameEngine::GameEngine()
	{
		m_Instance = this;
	}

	GameEngine::~GameEngine()
	{

	}

	void GameEngine::Start()
	{
		INFO("GameEngine 1.0. Created for Zombie by tecnessino. All rights reserved.");
		Time time;

		JobManager::Init();

		JPH::RegisterDefaultAllocator();
		JPH::Factory::sInstance = new JPH::Factory();
		JPH::RegisterTypes();

		LayerStackEmitter::EmitStart(GetLayerStack());

		while (!m_ShouldExit)
		{
			time.Update();

			const bool fixed = time.ShouldCallFixed();

			m_EventQueueMutex.lock();
			while (!m_EventQueue.empty())
			{
				Event* ev = m_EventQueue.front();
				m_EventQueue.pop();

				LayerStackEmitter::EmitEvent(GetLayerStack(), *ev);

				delete ev;
			}
			m_EventQueueMutex.unlock();

			{
				Hyper::UpdateEvent update;
				update.DeltaTime = time.GetDeltaTime();
				LayerStackEmitter::EmitEvent(GetLayerStack(), update);
			}

			if (fixed)
			{
				m_TickCount++;

				Hyper::FixedUpdateEvent update;
				LayerStackEmitter::EmitEvent(GetLayerStack(), update);
			}
		}

		LayerStackEmitter::EmitExit(GetLayerStack());

		JobManager::Dispose();

		INFO("Shutdowned gratefully");
	}

	void GameEngine::QueueEvent(Event* event)
	{
		std::lock_guard lock(m_EventQueueMutex);
		m_EventQueue.push(event);
	}

	uint64 GameEngine::GetTick() const
	{
		return m_TickCount;
	}

	void GameEngine::RequestExit()
	{
		m_ShouldExit = true;
	}

	LayerStack& GameEngine::GetLayerStack()
	{
		return m_LayerStack;
	}

	DeviceHelper& GameEngine::GetDeviceHelper()
	{
		return m_DeviceHelper;
	}
}
