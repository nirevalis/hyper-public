#include "FramebufferCache.hpp"

namespace Hyper
{
	std::unordered_map<uint64, nvrhi::FramebufferHandle> FramebufferCache::m_FramebufferCache;

	namespace Internal
	{
		uint64 GetHash(const nvrhi::FramebufferDesc& k)
		{
			std::size_t hash = 0;

			auto hash_combine = [] (std::size_t& seed, std::size_t value)
				{
					seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
				};

			for (const nvrhi::FramebufferAttachment& attachment : k.colorAttachments)
			{
				if (attachment.valid())
				{
					void* ptr = attachment.texture;
					hash_combine(hash, std::hash<void*>{}(ptr));
					hash_combine(hash, std::hash<unsigned int>{}(attachment.subresources.baseArraySlice));
					hash_combine(hash, std::hash<unsigned int>{}(attachment.subresources.baseMipLevel));
				}
				else
				{
					hash_combine(hash, 0);
				}
			}

			if (k.depthAttachment.valid())
			{
				void* ptr = k.depthAttachment.texture;
				hash_combine(hash, std::hash<void*>{}(ptr));
				hash_combine(hash, std::hash<unsigned int>{}(k.depthAttachment.subresources.baseArraySlice));
				hash_combine(hash, std::hash<unsigned int>{}(k.depthAttachment.subresources.baseMipLevel));
			}

			return hash;
		}
	}

	nvrhi::FramebufferHandle FramebufferCache::Get(nvrhi::DeviceHandle device, const nvrhi::FramebufferDesc& desc)
	{
		uint64 hash = Internal::GetHash(desc);

		if (!m_FramebufferCache.contains(hash))
			m_FramebufferCache.emplace(hash, device->createFramebuffer(desc));


		return m_FramebufferCache.at(hash);
	}
}