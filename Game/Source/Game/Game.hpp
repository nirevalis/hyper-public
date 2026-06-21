#pragma once

#include <Core/Common.hpp>
#include <Engine/Event.hpp>
#include <Engine/Layer.hpp>
#include <Renderer/Device.hpp>
#include <Renderer/Render2D.hpp>

#include "ActionSystem.hpp"
#include "GameRenderer.hpp"
#include "GameWindow.hpp"
#include "LocalPlayer.hpp"
#include "UserInterface.hpp"
#include "Maps/GymMap.hpp"
#include "Weapon/WeaponSystem.hpp"

namespace Hyper
{
    class HYPER_API Game
    {
    private:
        static Game* m_Instance;

    private:
        Scope<ILayer> m_GameImplLayer = nullptr;

        Scope<IMap> m_Map = nullptr;
        Scope<LocalPlayer> m_LocalPlayer = nullptr;
        Scope<ActionSystem> m_ActionSystem = nullptr;
        Scope<WeaponSystem> m_WeaponSystem;

        //Default gapi is d3d12.
        GraphicsAPI m_GraphicsAPI = GraphicsAPI::D3D12;
        Scope<IDevice> m_DeviceImpl = nullptr;
        Scope<ISwapchain> m_SwapchainImpl = nullptr;
        nvrhi::DeviceHandle m_Device = nullptr;
        nvrhi::CommandListHandle m_MainContext = nullptr;
        Scope<GameWindow> m_GameWindow = nullptr;
        Float2 m_ResizeTarget = Float2::Zero;

        Scope<GameRenderer> m_GameRenderer = nullptr;
        Scope<UserInterface> m_UserInterface = nullptr;

        void createDevice();
    public:
        static Game& Get()
        {
            return *m_Instance;
        }

    public:
        Game();
        ~Game();

        ILayer* GetGameLayer() const;
        nvrhi::DeviceHandle GetDevice() const;
        LocalPlayer* GetLocalPlayer() const;
        GameInput& GetGameInput() const;
        GameWindow* GetGameWindow() const;
        ActionSystem* GetActionSystem() const;

        void Start();
        void Dispose();
        void OnEvent(Event& event);
    };
}
