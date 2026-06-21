#include "SceneRenderer.hpp"

#include "Engine/GameEngine.hpp"

namespace Hyper
{
	SceneRenderer::SceneRenderer(class Scene* scene, ScriptablePipeline& pipeline) : Scene(scene), Pipeline(pipeline)
	{
	}

	void SceneRenderer::Render(nvrhi::CommandListHandle context, nvrhi::FramebufferHandle output)
	{
		EulerTransform& transform = Scene->Camera.GetTransform();

		DrawCallCollectContext collectContext {};

		SceneDrawEvent event(collectContext);
		Scene->OnEvent(event);

		ScriptablePipelineRenderingContext pipelineContext = {};
		pipelineContext.Context = context;
		pipelineContext.Device = GameEngine::Get().GetDeviceHelper().GetDevice();
		pipelineContext.Output = output;
		pipelineContext.Viewport = Float2(output->getDesc().colorAttachments[0].texture->getDesc().width, output->getDesc().colorAttachments[0].texture->getDesc().height);
		pipelineContext.DrawCalls = collectContext.m_DrawCalls;

		Pipeline.Render(pipelineContext, Scene->Camera);
	}
}
