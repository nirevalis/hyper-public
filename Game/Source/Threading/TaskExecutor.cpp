#include "TaskExecutor.hpp"

namespace Hyper
{
	TaskExecutorInfo TaskExecutor::GetInfo()
	{
		return { m_ExecutorIndex, m_Importance };
	}

	TaskExecutorInfo::TaskExecutorInfo(int32 index, TaskImportance importance)
		: Index(index),
		Importance(importance)
	{

	}


	TaskExecutor::TaskExecutor(TaskImportance importance, TaskExecutorConsumeData* data, int executorIndex) : m_Initialized(true),
		m_Importance(importance),
		m_ConsumeData(data),
		m_ExecutorIndex(executorIndex),
		m_ShouldExit(false),
		m_Thread(threadMain, this)
	{
	};

	TaskExecutor::~TaskExecutor()
	{
		if (!m_Initialized)
			return;

		m_ShouldExit = true;

		{
			std::lock_guard<std::mutex> lock(m_ConsumeData->Locker);
		}

		m_ConsumeData->QueueCV.notify_all();
		if (m_Thread.joinable())
			m_Thread.join();
	}

	void TaskExecutor::threadMain(TaskExecutor* executor)
	{
		TaskExecutorConsumeData& consume = *executor->m_ConsumeData;

		while (!executor->m_ShouldExit)
		{
			std::unique_lock<std::mutex> lock(consume.Locker);
			consume.QueueCV.wait(lock, [&]
				{
					return !consume.Queue.empty() || executor->m_ShouldExit;
				});

			if (!consume.Queue.empty())
			{
				TaskGraph graph = consume.Queue.front();
				consume.Queue.pop();
				lock.unlock();

				graph.Execute(executor);
			}
		}
	}
}