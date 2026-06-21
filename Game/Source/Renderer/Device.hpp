#pragma once

#include <Core/Common.hpp>
#include <nvrhi/nvrhi.h>
#include <functional>

namespace Hyper
{
	enum class GraphicsAPI : uint8
	{
		D3D12 = 0
	};

	class HYPER_API ISwapchain
	{
	public:
		ISwapchain() = default;
		virtual ~ISwapchain() = default;

		virtual void BeginFrame() = 0;
		virtual void Present() = 0;
		virtual nvrhi::TextureHandle GetCurrentImage() = 0;
		virtual nvrhi::TextureHandle GetImage(uint32 index) = 0;
		virtual uint32 GetCurrentImageIndex() = 0;
		virtual void Resize(int32 width, int32 height) = 0;
	};

	class HYPER_API DeviceHelper
	{
	private:
		nvrhi::DeviceHandle m_Device;
	public:
		void SetDevice(nvrhi::DeviceHandle handle)
		{
			m_Device = handle;
		}

		nvrhi::DeviceHandle GetDevice() const
		{
			return m_Device;
		}

		void SubmitCopy(const std::function<void(nvrhi::CommandListHandle)>& commands)
		{
			nvrhi::CommandListParameters params {};
			params.setQueueType(nvrhi::CommandQueue::Copy);

			auto cmdList = m_Device->createCommandList(params);
			cmdList->open();
			commands(cmdList);
			cmdList->close();

			m_Device->executeCommandList(cmdList, nvrhi::CommandQueue::Copy);
		}

		void Submit(const std::function<void(nvrhi::CommandListHandle)>& commands)
		{
			nvrhi::CommandListParameters params {};
			params.setQueueType(nvrhi::CommandQueue::Graphics);

			auto cmdList = m_Device->createCommandList(params);
			cmdList->open();
			commands(cmdList);
			cmdList->close();

			m_Device->executeCommandList(cmdList, nvrhi::CommandQueue::Graphics);
		}
	};

	class HYPER_API IDevice
	{
	public:
		IDevice() = default;
		virtual ~IDevice() = default;

		virtual nvrhi::DeviceHandle CreateDevice() = 0;
		virtual ISwapchain* CreateSwapchain(void* windowHandle) = 0;
		virtual void ReportLiveObjects() = 0;
	};
}