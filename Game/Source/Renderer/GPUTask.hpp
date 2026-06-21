#pragma once

#include <Core/Common.hpp>
#include <Threading/Task.hpp>
#include <nvrhi/nvrhi.h>

namespace Hyper
{
	class GPUTask : public Task
	{
	public:
		struct GPUTaskExecutionInfo
		{
			nvrhi::CommandQueue QueueType;
		};

		void Run() override;

		virtual void Compute(nvrhi::CommandListHandle cmd) = 0;
		virtual GPUTaskExecutionInfo GetExecutionInfo() = 0;
	};
}
