#include "JobManager.hpp"

namespace Hyper
{
	void JobManager::Init()
	{
		m_MainExecutor.CreateExecutors();
		m_SideExecutor.CreateExecutors();
	}

	void JobManager::Run(TaskImportance importance, const TaskGraph& graph)
	{
		if (importance == Main)
			m_MainExecutor.Run(graph);
		else m_SideExecutor.Run(graph);
	}

	void JobManager::Dispose()
	{
		m_MainExecutor.~TaskExecutorGroup();
		m_SideExecutor.~TaskExecutorGroup();
	}
}