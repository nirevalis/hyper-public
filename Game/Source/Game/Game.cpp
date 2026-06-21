#include "Game.hpp"

#include <Asset/AssetRegistry.hpp>
#include <Engine/EventDispatcher.hpp>
#include <Engine/GameEngine.hpp>
#include <Renderer/FramebufferCache.hpp>
#include <Renderer/ShaderLibrary.hpp>
#include <Renderer/Devices/D3D12DeviceImpl.hpp>

#include "Entities/FallingEntity.hpp"
#include "Entities/StaticModelEntity.hpp"

namespace Hyper
{
	namespace Branding
	{
		static String GameName = "Game";
		static String Version = "2025.06";
	}

    Game* Game::m_Instance = nullptr;

    class GameProxyLayer : public Hyper::ILayer
    {
    public:
		Game& Game;

    	GameProxyLayer(::Hyper::Game& game) : Game(game)
    	{

    	}

		void OnStart() override
		{
    		Game.Start();
		}

		void OnExit() override
		{
			Game.Dispose();
		}

		void OnEvent(Hyper::Event& event) override
		{
    		Game.OnEvent(event);
		}
    };

    Game::Game() : m_GameImplLayer(CreateScope<GameProxyLayer>(*this))
    {
        m_Instance = this;
    }

	Game::~Game()
    {

    }

	void Game::createDevice()
    {
		switch (m_GraphicsAPI)
		{
			case GraphicsAPI::D3D12:
				m_DeviceImpl = CreateScope<D3D12DeviceImpl>();
				m_Device = m_DeviceImpl->CreateDevice();

				m_SwapchainImpl.reset(m_DeviceImpl->CreateSwapchain(m_GameWindow->GetNativePointer()));
				m_GameWindow->OnResize.Connect([this](int32 width, int32 height)
				{
					m_ResizeTarget = {static_cast<float>(width),static_cast<float>(height)};
				});
				break;
		}

    	m_MainContext = m_Device->createCommandList();
    	GameEngine::Get().GetDeviceHelper().SetDevice(m_Device);

    	ShaderLibrary::LoadFrom(m_Device, "D:/Hyper/Game/Compiled/Shaders");
    }

    void Game::Start()
    {
		INFO("Starting {} {}", Branding::GameName, Branding::Version);

    	m_GameWindow = CreateScope<GameWindow>(
				FORMAT("{} {}", Branding::GameName, Branding::Version),
				Float2{1920,1080},
				false);
    	createDevice();

    	m_UserInterface = CreateScope<UserInterface>();
    	m_GameRenderer = CreateScope<GameRenderer>();
    	m_ActionSystem = CreateScope<ActionSystem>();
    	m_WeaponSystem = CreateScope<WeaponSystem>();

    	AssetRegistry::LoadAndFlush("Resources/Models/Cube.hpr");
    	AssetRegistry::LoadAndFlush("Resources/Models/Cylinder.hpr");
    	AssetRegistry::LoadAndFlush("Resources/Models/Sphere.hpr");
    	AssetRegistry::LoadAndFlush("Resources/Models/Stairs.hpr");

    	AssetRegistry::LoadAndFlush("Resources/Models/Doors/House/Door.hpr");
    	AssetRegistry::LoadAndFlush("Resources/Models/Doors/House/Frame.hpr");

    	AssetRegistry::LoadAndFlush("Resources/Models/Props/Mailbox.hpr");

    	AssetRegistry::LoadAndFlush("Resources/HDR/Irradiance.hpr");
    	AssetRegistry::LoadAndFlush("Resources/HDR/Radiance.hpr");

    	m_Map = CreateScope<GymMap>();

    	m_LocalPlayer = CreateScope<LocalPlayer>();
    	m_Map->Load(m_LocalPlayer->Scene);
    }

    void Game::Dispose()
    {

    }

    void Game::OnEvent(Event &event)
    {
		if (event.GetEventType() == EventType::Update)
		{
			auto& updateEvent = static_cast< UpdateEvent& >(event);

			m_ActionSystem->Update(m_LocalPlayer->Scene);
			m_WeaponSystem->Update(updateEvent.DeltaTime);

			m_GameWindow->BeginFrame();

			if (m_GameWindow->ShouldClose())
				GameEngine::Get().RequestExit();

			m_GameRenderer->Render(
				m_MainContext,
				m_SwapchainImpl.get(),
				m_GameWindow.get(),
				m_UserInterface.get(),
				m_LocalPlayer->Scene,
				updateEvent.DeltaTime
				);

			m_Device->runGarbageCollection();

			if (m_ResizeTarget != Float2::Zero)
			{
				m_SwapchainImpl->Resize(m_ResizeTarget.X, m_ResizeTarget.Y);
				m_ResizeTarget = Float2::Zero;
			}

			m_GameWindow->EndFrame();
		}

    	m_LocalPlayer->Scene->OnEvent(event);
    }

	ILayer* Game::GetGameLayer() const
    {
	    return m_GameImplLayer.get();
    }

	nvrhi::DeviceHandle Game::GetDevice() const
	{
		return m_Device;
	}

	LocalPlayer* Game::GetLocalPlayer() const
    {
	    return m_LocalPlayer.get();
    }

	GameInput &Game::GetGameInput() const
	{
		return m_GameWindow->Input;
	}

	ActionSystem *Game::GetActionSystem() const
    {
	    return m_ActionSystem.get();
    }

	GameWindow *Game::GetGameWindow() const
    {
	    return m_GameWindow.get();
    }
}
