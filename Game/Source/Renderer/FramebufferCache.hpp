#pragma once

#include <Core/Common.hpp>
#include <nvrhi/nvrhi.h>
#include <unordered_map>

namespace Hyper
{
	class HYPER_API FramebufferCache
	{
	private:
		static std::unordered_map<uint64, nvrhi::FramebufferHandle> m_FramebufferCache;

	public:
		static nvrhi::FramebufferHandle Get(nvrhi::DeviceHandle device, const nvrhi::FramebufferDesc& desc);
	};
}