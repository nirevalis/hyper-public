#pragma once

#include <Core/Common.hpp>
#include <Renderer/Font.hpp>
#include <Renderer/Render2D.hpp>

namespace Hyper
{
    class IScreen
    {
    public:
        IScreen() = default;
        virtual ~IScreen() = default;

        virtual void Render(Render2D& renderer, nvrhi::CommandListHandle cmd) = 0;
    };

    class UserInterface
    {
    private:
        Scope<Render2D> m_Renderer;
        Font m_RobotoFont;

        float m_CurtainAlpha = 1.0f;

    public:
        UserInterface();
        ~UserInterface() = default;

        void DrawCurtain(MinimalGraphicsContext& context);
        void DrawDebugTexts(MinimalGraphicsContext& context);
        void DrawCrosshair(MinimalGraphicsContext& context);
        void DrawActions(MinimalGraphicsContext& context);

        void Draw(MinimalGraphicsContext& context);

        Render2D* GetRenderer() const { return m_Renderer.get(); }
    };
}
