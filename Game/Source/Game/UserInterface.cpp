#include "UserInterface.hpp"

#include <Engine/GameEngine.hpp>

#include "Game.hpp"

namespace Hyper
{
    UserInterface::UserInterface()
        : m_Renderer(CreateScope<Render2D>(GameEngine::Get().GetDeviceHelper().GetDevice())),
        m_RobotoFont("Resources/Fonts/Roboto.ttf")
    {

    }

    void UserInterface::DrawCurtain(MinimalGraphicsContext &context)
    {
        const Float2& viewportSize = {
            context.State.viewport.viewports[0].width(),
            context.State.viewport.viewports[0].height()
        };

        m_Renderer->DrawRect(Rectangle{0,0,viewportSize.X,viewportSize.Y}, Color::FromRGBA(0.0f,0.0f,0.0f,m_CurtainAlpha));
        m_CurtainAlpha += (0.0f - m_CurtainAlpha) * (context.DeltaTime / 2.5f);
    }


    void UserInterface::DrawDebugTexts(MinimalGraphicsContext& context)
    {
        const Float2& viewportSize = {
            context.State.viewport.viewports[0].width(),
            context.State.viewport.viewports[0].height()
        };

        //Work in progress text
        m_Renderer->DrawText("WORK IN PROGRESS", Float2(viewportSize.X / 2, 5), Color::FromRGBA(255,255,255, 120), m_RobotoFont, 28, true);

        m_Renderer->DrawRoundedRect(Rectangle{2,2, 450,134}, Color::FromRGBA(32,32,32, 120), 16);

        Float3 playerPos = Game::Get().GetLocalPlayer()->Player->GetTransform().Position;

        float yLevel = 10;
        m_Renderer->DrawText("Game 2025.06", Float2{10, yLevel}, Color::FromRGB(255,255,255), m_RobotoFont, 24);
        m_Renderer->DrawText(FORMAT("Tick: {}", GameEngine::Get().GetTick()), Float2{10, yLevel+=24}, Color::FromRGB(255,255,255), m_RobotoFont, 24);
        m_Renderer->DrawText(FORMAT("DeltaTime: {}", context.DeltaTime), Float2{10, yLevel+=24}, Color::FromRGB(255,255,255), m_RobotoFont, 24);
        m_Renderer->DrawText(FORMAT("Position: {} {} {}",playerPos.X, playerPos.Y, playerPos.Z), Float2{10, yLevel+=24}, Color::FromRGB(255,255,255), m_RobotoFont, 24);
    }

    void UserInterface::DrawCrosshair(MinimalGraphicsContext &context)
    {
        const Float2& viewportSizeHalf = {
            context.State.viewport.viewports[0].width() * 0.5f,
            context.State.viewport.viewports[0].height() * 0.5f
        };
        m_Renderer->DrawRoundedRect(Rectangle{viewportSizeHalf.X-6, viewportSizeHalf.Y-6,12,12}, Color::FromRGBA(0,0,0,120), 6);
    }

    void UserInterface::DrawActions(MinimalGraphicsContext &context)
    {
        ActionSystem* system = Game::Get().GetActionSystem();
        auto& actions = system->GetActions();

        Float2 position(300,500);

        for (const auto& entry : actions)
        {
            m_Renderer->DrawText(entry.first, position, Color::FromRGB(255,255,255), m_RobotoFont, 18);
            position.Y+=18;

            for (const auto& action : entry.second)
            {
                m_Renderer->DrawText(FORMAT("[{}] {}", glfwGetKeyName(action.Keybind, 0), action.Text), position, Color::FromRGB(200,200,200), m_RobotoFont, 18);
                position.Y+=18;
            }
        }
    }

    void UserInterface::Draw(MinimalGraphicsContext& context)
    {
        const Float2& viewportSize = {
            context.State.viewport.viewports[0].width(),
            context.State.viewport.viewports[0].height()
        };

        m_Renderer->Begin(context);

        DrawDebugTexts(context);
        DrawCrosshair(context);
        DrawActions(context);

        if (m_CurtainAlpha > 0)
            DrawCurtain(context);

        m_Renderer->Execute(context);
    }
}
