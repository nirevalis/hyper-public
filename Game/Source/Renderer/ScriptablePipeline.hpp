#pragma once

#include <Core/Common.hpp>
#include <nvrhi/nvrhi.h>
#include "Camera.hpp"
#include "DrawContext.hpp"

namespace Hyper
{
	struct HYPER_API MinimalGraphicsContext
	{
		nvrhi::DeviceHandle Device;
		nvrhi::CommandListHandle Context;
		nvrhi::GraphicsState State;
		double DeltaTime = 0;
	};

	class HYPER_API ScriptablePipelineRenderingContext
	{
	public:
		nvrhi::DeviceHandle Device;
		nvrhi::CommandListHandle Context;
		nvrhi::FramebufferHandle Output;
		Float2 Viewport;
		std::vector<DrawCall> DrawCalls;
	};

	class HYPER_API ScriptablePipeline
	{
	public:
		ScriptablePipeline() = default;
		virtual ~ScriptablePipeline() = default;

		virtual void Render(ScriptablePipelineRenderingContext& context, Camera& camera) = 0;
	};
}
