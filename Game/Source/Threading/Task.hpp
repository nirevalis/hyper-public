#pragma once

#include <Core/Common.hpp>
#include <mutex>
#include <thread>

namespace Hyper
{
	class TaskExecutor;

	enum TaskImportance : uint8
	{
		Main = 0,
		Side
	};

	struct ProgressInfo
	{
		int DoneSteps;
		int AllSteps;
		std::string Text;
	};

	class Task
	{
	private:
		friend class TaskGraph;

		std::mutex m_ProgressLocker;
		ProgressInfo m_Progress;
		std::chrono::duration<double> m_Duration;

	protected:
		TaskExecutor* m_Executor;
		std::atomic_bool m_IsFinished = false;

		void UpdateProgress(const ProgressInfo& info);
	public:
		Task() = default;
		virtual ~Task() = default;

		std::chrono::duration<double> GetTaskDuration() const;
		ProgressInfo GetProgress();

		virtual void Run() = 0;

		static void Wait(std::initializer_list<Task*> tasks);
	};
}