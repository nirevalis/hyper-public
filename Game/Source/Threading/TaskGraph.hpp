#pragma once

#include <Core/Common.hpp>
#include <Core/Event.hpp>
#include <vector>
#include <chrono>
#include "Task.hpp"

namespace Hyper
{
	class TaskGraph
	{
	private:
		std::vector<Task*> m_Tasks;
		Delegate<> m_OnFinished;
		Delegate<> m_OnBeforeCall;

	public:
		template <typename T>
		FORCE_INLINE TaskGraph AddTask(T* task, void** outTask)
		{
			*outTask = task;
			m_Tasks.push_back(task);
			return *this;
		}

		TaskGraph AddOnFinished(Delegate<>::Callback&& callback);
		TaskGraph AddOnBeforeCall(Delegate<>::Callback&& callback);

		void Execute(TaskExecutor* executor) const;
	};
}
