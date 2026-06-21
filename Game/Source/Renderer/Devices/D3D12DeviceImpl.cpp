#include "D3D12DeviceImpl.hpp"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <nvrhi/d3d12.h>
#include <iostream>
#include <Engine/GameEngine.hpp>
#include <nvrhi/validation.h>
#include <dxgidebug.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "dxguid.lib")


#define CheckHR(expr) { HRESULT hr = expr; if(hr != S_NORMAL) ERR("HR != S_NORMAL. HR == {:#x}: {}", (uint32)hr, #expr); }

namespace Hyper
{
	struct D3D12MessengerImpl : public nvrhi::IMessageCallback
	{
		void message(nvrhi::MessageSeverity severenity, const char* text)
		{
			ERR("{}", text);
		};
	};

	struct D3D12PrivateImplementation
	{
		IDXGIFactory6* Factory = nullptr;
		IDXGIAdapter1* Adapter = nullptr;
		ID3D12Device* Device;
		IDXGIDebug1* Debug;
		ID3D12CommandQueue* CommandQueue;
		ID3D12CommandQueue* CopyCommandQueue;
		ID3D12CommandQueue* ComputeCommandQueue;
		D3D12MessengerImpl Messenger;
		nvrhi::DeviceHandle NVDevice;

		//	D3D12PrivateImplementation(const D3D12PrivateImplementation&) = delete;

		~D3D12PrivateImplementation()
		{
			NVDevice->waitForIdle();
			NVDevice->runGarbageCollection();
			CommandQueue->Release();
			Device->Release();
			Adapter->Release();
			Factory->Release();
		}
	};

	D3D12DeviceImpl::D3D12DeviceImpl() : m_pImpl(CreateScope<D3D12PrivateImplementation>())
	{
	}

	D3D12DeviceImpl::~D3D12DeviceImpl()
	{
	}

	nvrhi::DeviceHandle D3D12DeviceImpl::CreateDevice()
	{
		CheckHR(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_pImpl->Factory)));

		for (uint32 i = 0; ; ++i)
		{
			IDXGIAdapter1* tempAdapter = nullptr;
			if (m_pImpl->Factory->EnumAdapters1(i, &tempAdapter) == DXGI_ERROR_NOT_FOUND)
				break;

			DXGI_ADAPTER_DESC1 desc;
			tempAdapter->GetDesc1(&desc);

			if ((desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0)
			{
				m_pImpl->Adapter = tempAdapter;
				break;
			}

			tempAdapter->Release();
		}

		if (!m_pImpl->Adapter)
		{
			ERR("No suitable adapter found!");
			m_pImpl->Factory->Release();
		}

		ID3D12Debug* debugController = nullptr;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			debugController->Release();
		}

		DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_pImpl->Debug));

		CheckHR(D3D12CreateDevice(m_pImpl->Adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_pImpl->Device)));

		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

		CheckHR(m_pImpl->Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pImpl->CommandQueue)));

		D3D12_COMMAND_QUEUE_DESC copyQueueDesc = {};
		copyQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		copyQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		copyQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

		CheckHR(m_pImpl->Device->CreateCommandQueue(&copyQueueDesc, IID_PPV_ARGS(&m_pImpl->CopyCommandQueue)));

		D3D12_COMMAND_QUEUE_DESC computeQueueDesc = {};
		computeQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		computeQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		computeQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

		CheckHR(m_pImpl->Device->CreateCommandQueue(&computeQueueDesc, IID_PPV_ARGS(&m_pImpl->ComputeCommandQueue)));

		nvrhi::d3d12::DeviceDesc deviceDesc = {};
		deviceDesc.pDevice = m_pImpl->Device;
		deviceDesc.pGraphicsCommandQueue = m_pImpl->CommandQueue;
		deviceDesc.pCopyCommandQueue = m_pImpl->CopyCommandQueue;
		deviceDesc.pComputeCommandQueue = m_pImpl->ComputeCommandQueue;
		deviceDesc.errorCB = &m_pImpl->Messenger;
		deviceDesc.aftermathEnabled = true;

		INFO("Creating D3D12DeviceImpl.");
		return m_pImpl->NVDevice = nvrhi::validation::createValidationLayer(nvrhi::d3d12::createDevice(deviceDesc));
	}

	void D3D12DeviceImpl::ReportLiveObjects()
	{
		m_pImpl->Debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
	}

	ISwapchain* D3D12DeviceImpl::CreateSwapchain(void* windowHandle)
	{
		return new D3D12SwapchainImpl(*(m_pImpl.get()), windowHandle);
	}

	D3D12SwapchainImpl::D3D12SwapchainImpl(D3D12PrivateImplementation& impl, void* windowHandle)
	{
		m_Window = windowHandle;
		m_GraphicsCommandQueue = impl.CommandQueue;
		m_Device = impl.Device;
		m_Debug = impl.Debug;

		HWND hwnd = static_cast< HWND >(windowHandle);
		uint32 windowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

		//RECT rect = { 0, 0, LONG(1920), LONG(1080) };
		//AdjustWindowRect(&rect, windowStyle, FALSE);

		RECT clientRect;
		GetClientRect(hwnd, &clientRect);
		UINT width = clientRect.right - clientRect.left;
		UINT height = clientRect.bottom - clientRect.top;

		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Width = width;
		desc.Height = height;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 2;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		IDXGISwapChain1* swapChain;
		impl.Factory->CreateSwapChainForHwnd(impl.CommandQueue, hwnd, &desc, nullptr, nullptr, &swapChain);
		swapChain->QueryInterface(IID_PPV_ARGS(&m_SwapChain));

		m_SwapChainImages.resize(desc.BufferCount);
		m_NVImages.resize(desc.BufferCount);

		for (uint32 bufferIndex = 0; bufferIndex < desc.BufferCount; bufferIndex++)
		{
			CheckHR(m_SwapChain->GetBuffer(bufferIndex, IID_PPV_ARGS(&m_SwapChainImages[bufferIndex])));

			nvrhi::TextureDesc textureDesc;
			textureDesc.width = desc.Width;
			textureDesc.height = desc.Height;
			textureDesc.sampleCount = desc.SampleDesc.Count;
			textureDesc.sampleQuality = desc.SampleDesc.Quality;
			textureDesc.format = nvrhi::Format::RGBA8_UNORM;
			textureDesc.debugName = "SwapChainBuffer";
			textureDesc.isRenderTarget = true;
			textureDesc.isUAV = false;
			textureDesc.initialState = nvrhi::ResourceStates::Present;
			textureDesc.keepInitialState = true;

			m_NVImages[bufferIndex] = impl.NVDevice->createHandleForNativeTexture(nvrhi::ObjectTypes::D3D12_Resource, nvrhi::Object(m_SwapChainImages[bufferIndex]), textureDesc);
		}

		CheckHR(impl.Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
		m_FenceValue = 0;
		m_LastSubmittedFenceValue = 0;
		m_Fence->Signal(0);
		m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	}

	void D3D12SwapchainImpl::Resize(int32 width, int32 height)
	{
		nvrhi::DeviceHandle device = GameEngine::Get().GetDeviceHelper().GetDevice();
		device->waitForIdle();
		device->runGarbageCollection();


		SetEvent(m_FenceEvent);

		m_Debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		ID3D12DebugDevice* debugDevice;
		if (SUCCEEDED(m_Device->QueryInterface(IID_PPV_ARGS(&debugDevice))))
		{
			debugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
		}

		m_NVImages.clear();
		m_SwapChainImages.clear();

		DXGI_SWAP_CHAIN_DESC desc = {};
		m_SwapChain->GetDesc(&desc);
		CheckHR(m_SwapChain->ResizeBuffers(desc.BufferCount, width, height,
											  desc.BufferDesc.Format, desc.Flags));

		m_SwapChainImages.resize(desc.BufferCount);
		m_NVImages.resize(desc.BufferCount);

		for (uint32 bufferIndex = 0; bufferIndex < desc.BufferCount; bufferIndex++)
		{
			CheckHR(m_SwapChain->GetBuffer(bufferIndex, IID_PPV_ARGS(&m_SwapChainImages[bufferIndex])));

			nvrhi::TextureDesc textureDesc;
			textureDesc.width = width;
			textureDesc.height = height;
			textureDesc.sampleCount = desc.SampleDesc.Count;
			textureDesc.sampleQuality = desc.SampleDesc.Quality;
			textureDesc.format = nvrhi::Format::RGBA8_UNORM;
			textureDesc.debugName = "SwapChainBuffer";
			textureDesc.isRenderTarget = true;
			textureDesc.isUAV = false;
			textureDesc.initialState = nvrhi::ResourceStates::Present;
			textureDesc.keepInitialState = true;

			m_NVImages[bufferIndex] = GameEngine::Get().GetDeviceHelper().GetDevice()->createHandleForNativeTexture(nvrhi::ObjectTypes::D3D12_Resource, nvrhi::Object(m_SwapChainImages[bufferIndex]), textureDesc);
		}
	}

	D3D12SwapchainImpl::~D3D12SwapchainImpl()
	{
		m_NVImages.clear();
		m_SwapChainImages.clear();
	}

	void D3D12SwapchainImpl::BeginFrame()
	{
		m_CurrentBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

		if (m_Fence->GetCompletedValue() < m_FenceValue)
		{
			m_Fence->SetEventOnCompletion(m_FenceValue, m_FenceEvent);
			WaitForSingleObject(m_FenceEvent, INFINITE);
		}

		m_FenceValue++;
	}


	void D3D12SwapchainImpl::Present()
	{
		m_GraphicsCommandQueue->Signal(m_Fence, m_FenceValue);
		m_SwapChain->Present(0, 0);
	}

	nvrhi::TextureHandle D3D12SwapchainImpl::GetCurrentImage()
	{
		return m_NVImages[m_SwapChain->GetCurrentBackBufferIndex()];
	}

	nvrhi::TextureHandle D3D12SwapchainImpl::GetImage(uint32 index)
	{
		return m_NVImages[index];
	}

	uint32 D3D12SwapchainImpl::GetCurrentImageIndex()
	{
		return m_CurrentBackBufferIndex;
	}
}