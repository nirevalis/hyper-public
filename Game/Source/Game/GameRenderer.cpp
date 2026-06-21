#include "GameRenderer.hpp"

#include <nvrhi/utils.h>
#include <Renderer/FramebufferCache.hpp>

namespace Hyper
{
    GameRenderer::GameRenderer() : Renderer(nullptr, Pipeline)
    {
    }

    GameRenderer::~GameRenderer()
    {

    }

    void GameRenderer::Render(nvrhi::CommandListHandle cmd,
            ISwapchain* swapchain,
            GameWindow* window,
            UserInterface* ui,
            Scene* scene,
            double deltaTime)
    {
        nvrhi::DeviceHandle device = cmd->getDevice();
        swapchain->BeginFrame();
//f
        MinimalGraphicsContext context {};
        context.DeltaTime = deltaTime;
        context.Context = cmd;
        context.Device = device;
        context.State.setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport(window->Size.X, window->Size.Y)));
        context.State.setFramebuffer(FramebufferCache::Get(device, nvrhi::FramebufferDesc().addColorAttachment(swapchain->GetCurrentImage())));

        cmd->open();
        nvrhi::utils::ClearColorAttachment(cmd, context.State.framebuffer, 0, nvrhi::Color{0,0,0,0});

        Renderer.Scene = scene;
        Renderer.Render(cmd, context.State.framebuffer);

        ui->Draw(context);

        cmd->close();

        device->executeCommandList(cmd);
        swapchain->Present();
    }

}
