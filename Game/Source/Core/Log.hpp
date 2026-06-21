#pragma once

#include <format>
#include "Macros.hpp"

#define INFO(...) Hyper::Log::Info(__FUNCTION__, std::format(__VA_ARGS__).c_str())
#define ERR(...) Hyper::Log::Error(__FUNCTION__, std::format(__VA_ARGS__).c_str())

namespace Hyper
{
	class HYPER_API Log
	{
	public:
		Log() = delete;
		~Log() = delete;

		static void Info(const char* func, const char* str);
		static void Error(const char* func, const char* str);
	};
}