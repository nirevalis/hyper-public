#include "GPUTask.hpp"

#include "Engine/GameEngine.hpp"
#include "Threading/TaskExecutor.hpp"
#include <d3d12.h>

namespace Hyper
{
	static std::mutex CommandPoolMutex;
	static std::queue<nvrhi::CommandListHandle> CommandPool;
	static std::queue<nvrhi::CommandListHandle> CopyCommandPool;
	static std::queue<nvrhi::CommandListHandle> ComputeCommandPool;
	static bool PoolInitialized = false;

	void GPUTask::Run()
	{
		const GPUTaskExecutionInfo& info = GetExecutionInfo();
		nvrhi::DeviceHandle device = GameEngine::Get().GetDeviceHelper().GetDevice();

		if (!PoolInitialized)
		{
			nvrhi::CommandListParameters desc {};
			desc.enableImmediateExecution = false;

			for (uint32 i = 0; i < 8; i++)
				CommandPool.push(device->createCommandList(desc));

			desc.queueType = nvrhi::CommandQueue::Copy;

			for (uint32 i = 0; i < 8; i++)
				CopyCommandPool.push(device->createCommandList(desc));

			desc.queueType = nvrhi::CommandQueue::Compute;

			for (uint32 i = 0; i < 8; i++)
				ComputeCommandPool.push(device->createCommandList(desc));

			PoolInitialized = true;
		}

		std::queue < nvrhi::CommandListHandle>* pool;

		switch (info.QueueType)
		{
		case nvrhi::CommandQueue::Graphics:
			pool = &CommandPool;
			break;
		case nvrhi::CommandQueue::Copy:
			pool = &CopyCommandPool;
			break;
		case nvrhi::CommandQueue::Compute:
			pool = &ComputeCommandPool;
			break;
		}

		CommandPoolMutex.lock();
		auto cmd = pool->front(); pool->pop();
		CommandPoolMutex.unlock();

		cmd->open();
		Compute(cmd);
		cmd->close();

		uint64_t submission = device->executeCommandList(cmd, info.QueueType);

		device->waitCPUForExecution(submission, info.QueueType);

		CommandPoolMutex.lock();
		pool->push(cmd);
		CommandPoolMutex.unlock();
	}
}

