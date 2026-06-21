#pragma once

#include <Core/Common.hpp>
#include <Scene/Scene.hpp>
#include "ScriptablePipeline.hpp"
#include <nvrhi/nvrhi.h>

namespace Hyper
{
	class HYPER_API SceneRenderer
	{
	public:
		Scene* Scene;

		ScriptablePipeline& Pipeline;

		SceneRenderer(class Scene* scene, ScriptablePipeline& pipeline);
		~SceneRenderer() = default;

		void Render(nvrhi::CommandListHandle context, nvrhi::FramebufferHandle output);
	};
}