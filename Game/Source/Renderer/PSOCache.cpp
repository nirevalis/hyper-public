#include "FramebufferCache.hpp"
#include "PSOCache.hpp"

namespace Hyper
{
	namespace Internal
	{
		uint64 GetHash(nvrhi::IFramebuffer* framebuffer, const nvrhi::GraphicsPipelineDesc& desc)
		{
			std::size_t hash = 0;

			auto hash_combine = [] (std::size_t& seed, std::size_t value)
				{
					seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
				};

			nvrhi::FramebufferDesc fbDesc = framebuffer->getDesc();

			for (const nvrhi::FramebufferAttachment& attachment : fbDesc.colorAttachments)
			{
				hash_combine(hash, attachment.texture->getDesc().width);
				hash_combine(hash, attachment.texture->getDesc().height);
				hash_combine(hash, static_cast< std::size_t >(attachment.format));
			}


			hash_combine(hash, static_cast< std::size_t >(desc.primType));

			hash_combine(hash, std::hash<void*>{}(desc.VS.Get()));
			hash_combine(hash, std::hash<void*>{}(desc.PS.Get()));
			hash_combine(hash, std::hash<void*>{}(desc.GS.Get()));
			hash_combine(hash, std::hash<void*>{}(desc.HS.Get()));
			hash_combine(hash, std::hash<void*>{}(desc.DS.Get()));

			const auto& dss = desc.renderState.depthStencilState;
			hash_combine(hash, static_cast< std::size_t >(dss.depthTestEnable));
			hash_combine(hash, static_cast< std::size_t >(dss.depthWriteEnable));
			hash_combine(hash, static_cast< std::size_t >(dss.depthFunc));
			hash_combine(hash, static_cast< std::size_t >(dss.stencilEnable));
			hash_combine(hash, static_cast< std::size_t >(dss.stencilReadMask));
			hash_combine(hash, static_cast< std::size_t >(dss.stencilWriteMask));

			const auto& rs = desc.renderState.rasterState;
			hash_combine(hash, static_cast< std::size_t >(rs.cullMode));
			hash_combine(hash, static_cast< std::size_t >(rs.fillMode));
			hash_combine(hash, static_cast< std::size_t >(rs.frontCounterClockwise));
			hash_combine(hash, static_cast< std::size_t >(rs.depthBias));
			hash_combine(hash, static_cast< std::size_t >(rs.slopeScaledDepthBias));

			const auto& blendState = desc.renderState.blendState;
			hash_combine(hash, static_cast< std::size_t >(blendState.alphaToCoverageEnable));
			for (const auto& target : blendState.targets)
			{
				hash_combine(hash, static_cast< std::size_t >(target.blendEnable));
				hash_combine(hash, static_cast< std::size_t >(target.srcBlend));
				hash_combine(hash, static_cast< std::size_t >(target.destBlend));
				hash_combine(hash, static_cast< std::size_t >(target.blendOp));
				hash_combine(hash, static_cast< std::size_t >(target.srcBlendAlpha));
				hash_combine(hash, static_cast< std::size_t >(target.destBlendAlpha));
				hash_combine(hash, static_cast< std::size_t >(target.blendOpAlpha));
				hash_combine(hash, static_cast< std::size_t >(target.colorWriteMask));
			}

			if (desc.inputLayout != nullptr)
			{
				for (uint32 i = 0; i < desc.inputLayout->getNumAttributes(); i++)
				{
					nvrhi::VertexAttributeDesc elem = *(desc.inputLayout->getAttributeDesc(i));
					hash_combine(hash, static_cast< std::size_t >(elem.format));
					hash_combine(hash, static_cast< std::size_t >(elem.offset));
					hash_combine(hash, static_cast< std::size_t >(elem.elementStride));
					hash_combine(hash, static_cast< std::size_t >(i));
					hash_combine(hash, static_cast< std::size_t >(elem.isInstanced));
				}
			}

			return static_cast< uint64 >(hash);
		}

	}

	std::unordered_map<uint64, nvrhi::GraphicsPipelineHandle> PSOCache::m_PSOCache;

	nvrhi::GraphicsPipelineHandle PSOCache::Get(nvrhi::DeviceHandle device, nvrhi::IFramebuffer* framebuffer, const nvrhi::GraphicsPipelineDesc& desc)
	{
		uint64 hash = Internal::GetHash(framebuffer, desc);

		if (!m_PSOCache.contains(hash))
		{
			auto cwel = device->createGraphicsPipeline(desc, framebuffer);
			m_PSOCache.emplace(hash, cwel);
		}

		return m_PSOCache.at(hash);
	}
}