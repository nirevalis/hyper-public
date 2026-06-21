#pragma once

#include <Core/Common.hpp>
#include <array>
#include "Task.hpp"
#include "TaskExecutor.hpp"

namespace Hyper
{
	template <TaskImportance TImportance, uint16 NExecutors>
	class TaskExecutorGroup
	{
	private:
		friend class TaskExecutor;

		std::array<TaskExecutor, NExecutors> m_Executors;
		TaskExecutorConsumeData m_ConsumeData;

	public:
		TaskExecutorGroup() = default;
		~TaskExecutorGroup()
		{
			while (true)
			{
				std::lock_guard<std::mutex> lock(m_ConsumeData.Locker);
				if (m_ConsumeData.Queue.empty()) break;
			}
		}

		inline void LinkExecutors(const std::array<TaskExecutor, NExecutors>& executors)
		{
			m_Executors = executors;
		}

		inline void CreateExecutors()
		{
			for (uint16 i = 0; i < NExecutors; i++)
				new (&m_Executors[i]) TaskExecutor(TImportance, &m_ConsumeData, i);
		}

		inline void Run(const TaskGraph& graph)
		{
			{
				std::lock_guard<std::mutex> lock(m_ConsumeData.Locker);
				m_ConsumeData.Queue.push(graph);
			}
			m_ConsumeData.QueueCV.notify_one();
		}
	};
}
