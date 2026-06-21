#pragma once

#include <Core/Common.hpp>
#include "TaskExecutorGroup.hpp"

namespace Hyper
{
	class JobManager
	{
	private:
		inline static TaskExecutorGroup<Main, 1i16> m_MainExecutor;
		inline static TaskExecutorGroup<Side, 1i16> m_SideExecutor;
	public:
		JobManager() = delete;
		~JobManager() = delete;

		static void Init();
		static void Run(TaskImportance importance, const TaskGraph& graph);
		static void Dispose();
	};
}