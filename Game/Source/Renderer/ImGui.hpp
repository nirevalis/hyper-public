#pragma once

#include <nvrhi/nvrhi.h>
#include <imgui.h>

#include "ScriptablePipeline.hpp"

namespace Hyper
{
	class HYPER_API ImGuiNVRHI
	{
	private:
		nvrhi::DeviceHandle m_device;
		nvrhi::CommandListHandle m_commandList;

		nvrhi::ShaderHandle vertexShader;
		nvrhi::ShaderHandle pixelShader;
		nvrhi::InputLayoutHandle shaderAttribLayout;

		nvrhi::TextureHandle fontTexture;
		nvrhi::SamplerHandle fontSampler;

		nvrhi::BufferHandle vertexBuffer;
		nvrhi::BufferHandle indexBuffer;


		nvrhi::GraphicsPipelineDesc basePSODesc;
		nvrhi::BindingLayoutHandle bindingLayout;

		std::vector<ImDrawVert> vtxBuffer;
		std::vector<ImDrawIdx> idxBuffer;

		bool reallocateBuffer(nvrhi::BufferHandle& buffer, size_t requiredSize, size_t reallocateSize, const bool indexBuffer);
		void updateGeometry(ImGuiContext* imContext, MinimalGraphicsContext& context);
	public:
		void Init(nvrhi::IDevice* device);
		void UpdateFontTexture(ImGuiContext* imContext, MinimalGraphicsContext& context);
		void Render(ImGuiContext* imContext, MinimalGraphicsContext& context);
	};
}
