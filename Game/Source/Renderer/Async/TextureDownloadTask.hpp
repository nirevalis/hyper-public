#pragma once

#include <Core/Common.hpp>
#include <Engine/GameEngine.hpp>
#include <Renderer/GPUTask.hpp>

namespace Hyper
{
    class HYPER_API ITextureDownloadTaskBase : public GPUTask
    {
    public:
        nvrhi::TextureHandle Texture;

        ITextureDownloadTaskBase() = default;
        virtual ~ITextureDownloadTaskBase() = default;
    };

    template <typename TByte, uint32 BytesPerPixel>
    class HYPER_API TextureDownloadTask : public ITextureDownloadTaskBase
    {
    public:
        std::vector<TByte> Bytes;
        nvrhi::StagingTextureHandle Staging;

        TextureDownloadTask(nvrhi::TextureHandle texture)
        {
            nvrhi::DeviceHandle device = GameEngine::Get().GetDeviceHelper().GetDevice();

            nvrhi::TextureDesc textureDesc;
            textureDesc.width = texture->getDesc().width;
            textureDesc.height = texture->getDesc().height;
            textureDesc.format = nvrhi::Format::RGBA8_UNORM;
            textureDesc.initialState = nvrhi::ResourceStates::ShaderResource;
            textureDesc.keepInitialState = true;
            textureDesc.debugName = "Texture Download Staging Buffer";

            Staging = device->createStagingTexture(textureDesc, nvrhi::CpuAccessMode::Read);
            Texture = texture;
        }

        ~TextureDownloadTask() = default;

        void Compute(nvrhi::CommandListHandle cmd) override
        {
            cmd->copyTexture(Staging, {}, Texture, {});

            size_t rowStride = 0;
            void* map = cmd->getDevice()->mapStagingTexture(Staging, {}, nvrhi::CpuAccessMode::Read, &rowStride);

            if (map)
            {
                const uint32 width = Texture->getDesc().width;
                const uint32 height = Texture->getDesc().height;
                const uint32 rowSize = width * BytesPerPixel;

                Bytes.resize(width * height * BytesPerPixel);

                uint8_t* dst = Bytes.data();
                const uint8_t* src = static_cast<const uint8_t*>(map);

                for (uint32 y = 0; y < height; ++y)
                {
                    std::memcpy(dst + y * rowSize, src + y * rowStride, rowSize);
                }
            }

            cmd->getDevice()->unmapStagingTexture(Staging);
        }

        GPUTaskExecutionInfo GetExecutionInfo() override
        {
            GPUTaskExecutionInfo info = { };
            info.QueueType = nvrhi::CommandQueue::Graphics;

            return info;
        }
    };
}