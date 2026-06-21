#pragma once

#include <dxgidebug.h>
#include <Core/Common.hpp>
#include <Renderer/Device.hpp>

class IDXGISwapChain4;
class ID3D12Resource;
class ID3D12Fence;
class ID3D12CommandQueue;
class ID3D12Device;

namespace Hyper
{
	struct D3D12PrivateImplementation;

	class HYPER_API D3D12SwapchainImpl : public ISwapchain
	{
	private:
		uint32 m_CurrentBackBufferIndex;
		IDXGISwapChain4* m_SwapChain;
		ID3D12CommandQueue* m_GraphicsCommandQueue;
		ID3D12Device* m_Device;
		IDXGIDebug1* m_Debug;
		void* m_Window;
		std::vector<ID3D12Resource*> m_SwapChainImages;
		std::vector<nvrhi::TextureHandle> m_NVImages;

		ID3D12Fence* m_Fence = nullptr;
		void* m_FenceEvent = nullptr;
		uint64 m_FenceValue = 0;
		uint64 m_LastSubmittedFenceValue = 0;

	public:
		D3D12SwapchainImpl(D3D12PrivateImplementation& impl, void* windowHandle);
		~D3D12SwapchainImpl();

		void BeginFrame() override;
		void Present() override;
		nvrhi::TextureHandle GetCurrentImage() override;
		nvrhi::TextureHandle GetImage(uint32 index) override;
		uint32 GetCurrentImageIndex() override;
		void Resize(int32 width, int32 height) override;
	};

	class HYPER_API D3D12DeviceImpl : public IDevice
	{
	private:
		Scope<D3D12PrivateImplementation> m_pImpl;
	public:
		D3D12DeviceImpl();
		~D3D12DeviceImpl();

		D3D12DeviceImpl(const D3D12DeviceImpl&) = delete;

		nvrhi::DeviceHandle CreateDevice() override;
		ISwapchain* CreateSwapchain(void* windowHandle) override;
		void ReportLiveObjects() override;
	};
}