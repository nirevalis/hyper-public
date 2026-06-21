#pragma once

#include <Core/Common.hpp>
#include <nvrhi/nvrhi.h>
#include <Renderer/Device.hpp>
#include <Renderer/SceneRenderer.hpp>
#include <Renderer/Pipelines/HybridStyledPipeline.hpp>
#include "GameWindow.hpp"
#include "UserInterface.hpp"

namespace Hyper
{
    class GameRenderer
    {
    private:
    public:
        SceneRenderer Renderer;
        HybridStyledPipeline Pipeline;

        GameRenderer();
        ~GameRenderer();

        void Render(
            nvrhi::CommandListHandle cmd,
            ISwapchain* swapchain,
            GameWindow* window,
            UserInterface* ui,
            Scene* scene,
            double deltaTime
            );
    };
}
