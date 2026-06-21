#pragma once

#include <Core/Common.hpp>
#include <Renderer/ScriptablePipeline.hpp>
#include <nvrhi/nvrhi.h>

namespace Hyper
{
	class HYPER_API RasterPipeline : public ScriptablePipeline
	{
	public:
		RasterPipeline();
		~RasterPipeline() = default;

		void Render(ScriptablePipelineRenderingContext& context, Camera& camera);
	};
}