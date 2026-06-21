#include "FramebufferCache.hpp"
#include "PSOCache.hpp"
#include "DescriptorsCache.hpp"

namespace Hyper
{
	namespace Internal
	{
		uint64 GetHash(const nvrhi::BindingSetDesc& desc)
		{
			std::size_t hash = 0;

			auto hash_combine = [] (std::size_t& seed, std::size_t value)
				{
					seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
				};

			for (const nvrhi::BindingSetItem& item : desc.bindings)
			{
				hash_combine(hash, std::hash<int>()(static_cast< int >(item.type)));
				hash_combine(hash, std::hash<uint32_t>()(item.slot));
				hash_combine(hash, std::hash<uint32_t>()(item.arrayElement));

				hash_combine(hash, std::hash<void*>()(item.resourceHandle));
			}

			return static_cast< uint64 >(hash);
		}

	}

	std::unordered_map<uint64, nvrhi::BindingSetHandle> DescriptorsCache::m_DescriptorsCache;

	nvrhi::BindingSetHandle DescriptorsCache::Get(nvrhi::DeviceHandle device, nvrhi::BindingLayoutHandle layout, const nvrhi::BindingSetDesc& desc)
	{
		uint64 hash = Internal::GetHash(desc);

		if (!m_DescriptorsCache.contains(hash))
		{
			auto cwel = device->createBindingSet(desc, layout);
			m_DescriptorsCache.emplace(hash, cwel);
		}

		return m_DescriptorsCache.at(hash);
	}
}