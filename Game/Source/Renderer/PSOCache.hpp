#pragma once

#include <Core/Common.hpp>
#include <nvrhi/nvrhi.h>
#include <unordered_map>

namespace Hyper
{
	class HYPER_API PSOCache
	{
	private:
		struct PSOCacheEntry
		{
			nvrhi::IFramebuffer* Framebuffer;
			nvrhi::GraphicsPipelineDesc Description;
		};

		static std::unordered_map<uint64, nvrhi::GraphicsPipelineHandle> m_PSOCache;

	public:
		static nvrhi::GraphicsPipelineHandle Get(nvrhi::DeviceHandle device, nvrhi::IFramebuffer* framebuffer, const nvrhi::GraphicsPipelineDesc& desc);
	};
}