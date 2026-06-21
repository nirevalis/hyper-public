#pragma once

#include <Core/Common.hpp>
#include <nvrhi/nvrhi.h>
#include <unordered_map>

namespace Hyper
{
	class HYPER_API DescriptorsCache
	{
	private:
		static std::unordered_map<uint64, nvrhi::BindingSetHandle> m_DescriptorsCache;

	public:
		static nvrhi::BindingSetHandle Get(nvrhi::DeviceHandle device, nvrhi::BindingLayoutHandle layout, const nvrhi::BindingSetDesc& desc);
	};
}