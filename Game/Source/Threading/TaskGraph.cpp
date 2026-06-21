#include "TaskGraph.hpp"

namespace Hyper
{
	TaskGraph TaskGraph::AddOnFinished(Delegate<>::Callback&& callback)
	{
		m_OnFinished.Connect(std::move(callback));
		return *this;
	}

	TaskGraph TaskGraph::AddOnBeforeCall(Delegate<>::Callback&& callback)
	{
		m_OnBeforeCall.Connect(std::move(callback));
		return *this;
	}

	void TaskGraph::Execute(TaskExecutor* executor) const
	{
		m_OnBeforeCall.Fire();

		for (Task* task : m_Tasks)
		{
			task->m_Executor = executor;

			auto start = std::chrono::high_resolution_clock::now();

			task->Run();
			task->m_IsFinished.store(true);
			task->m_Duration = std::chrono::duration_cast< std::chrono::microseconds >(std::chrono::high_resolution_clock::now() - start);
		}

		m_OnFinished.Fire();
	}
}