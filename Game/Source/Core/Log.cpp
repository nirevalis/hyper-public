#pragma once

#include "Log.hpp"
#include <spdlog/spdlog.h>

namespace Hyper
{
	void Log::Info(const char* func, const char* str)
	{
		spdlog::info("[{}] {}", func, str);
	}

	void Log::Error(const char* func, const char* str)
	{
		spdlog::error("[{}] {}", func, str);
	}
}