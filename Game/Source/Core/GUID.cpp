#include "GUID.hpp"
#include <Windows.h>
#pragma comment(lib, "ole32.lib")

namespace Hyper
{
	Guid Guid::Random()
	{
		Guid g;
		if (CoCreateGuid(reinterpret_cast< GUID* >(&g)) != S_OK)
			ERR("CoCreateGuid != S_OK");

		return g;
	}

	String Guid::ToString() const
	{
		return std::format("{:0>8x}-{:0>8x}-{:0>8x}-{:0>8x}", A, B, C, D).c_str();
	}
}