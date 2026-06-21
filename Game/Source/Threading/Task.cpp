#include "Task.hpp"

namespace Hyper
{
	void Task::UpdateProgress(const ProgressInfo& info)
	{
		std::unique_lock<std::mutex> locker(m_ProgressLocker);
		m_Progress = info;
	}

	std::chrono::duration<double> Task::GetTaskDuration() const
	{
		return m_Duration;
	}

	ProgressInfo Task::GetProgress()
	{
		std::unique_lock<std::mutex> locker(m_ProgressLocker);
		return m_Progress;
	}

	void Task::Wait(std::initializer_list<Task*> tasks)
	{
		for (Task* task : tasks)
			while (!task->m_IsFinished.load(std::memory_order_acquire))
				std::this_thread::yield();
	}
}