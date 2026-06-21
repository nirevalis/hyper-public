#pragma once

#include <Core/Common.hpp>
#include "Task.hpp"
#include "TaskGraph.hpp"
#include <queue>

namespace Hyper
{
	struct TaskExecutorConsumeData
	{
		std::mutex Locker;
		std::queue<TaskGraph> Queue;
		std::condition_variable QueueCV;
	};

	struct TaskExecutorInfo
	{
		int32 Index;
		TaskImportance Importance;

		TaskExecutorInfo(int index, TaskImportance importance);
		~TaskExecutorInfo() = default;
	};

	class TaskExecutor
	{
	private:
		int m_ExecutorIndex;
		TaskImportance m_Importance;
		bool m_Initialized = false;
		std::atomic<bool> m_ShouldExit { false };
		TaskExecutorConsumeData* m_ConsumeData;
		std::thread m_Thread;

		static void threadMain(TaskExecutor* executor);

	public:
		TaskExecutor() = default;
		TaskExecutor(TaskImportance importance, TaskExecutorConsumeData* data, int executorIndex);
		TaskExecutor(const TaskExecutor&) = delete;
		~TaskExecutor();

		TaskExecutorInfo GetInfo();
	};
}