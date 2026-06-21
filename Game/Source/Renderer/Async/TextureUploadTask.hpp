#pragma once

#include <Core/Common.hpp>
#include <Renderer/GPUTask.hpp>

namespace Hyper
{
	class HYPER_API ITextureUploadTaskBase : public GPUTask
	{
	public:
		nvrhi::TextureHandle Texture;

		ITextureUploadTaskBase() = default;
		virtual ~ITextureUploadTaskBase() = default;
	};

	template <typename TByte, uint32 RowStride>
	class HYPER_API TextureUploadTask : public ITextureUploadTaskBase
	{
	public:
		std::vector<TByte> Bytes;

		TextureUploadTask(const std::vector<TByte>& bytes, nvrhi::TextureHandle texture)
			: Bytes(bytes)
		{
			Texture = texture;
		}

		~TextureUploadTask() = default;

		void Compute(nvrhi::CommandListHandle cmd) override
		{
			size_t rowStride = static_cast< size_t >(Texture->getDesc().width) * RowStride;
			cmd->writeTexture(Texture, 0, 0, Bytes.data(), rowStride);
		}

		GPUTaskExecutionInfo GetExecutionInfo() override
		{
			GPUTaskExecutionInfo info = { };
			info.QueueType = nvrhi::CommandQueue::Graphics;

			return info;
		}
	};
}