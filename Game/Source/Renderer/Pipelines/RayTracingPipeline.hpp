#pragma once

#include <Core/Common.hpp>
#include <Renderer/ScriptablePipeline.hpp>
#include <nvrhi/nvrhi.h>
#include <Renderer/DescriptorTableManager.hpp>

namespace Hyper
{
	struct InstanceBufferIndices
	{
		uint32 vertexBufferIndex;
		uint32 indexBufferIndex;
	};

	struct RTMaterial
	{
		Float4 Color;
		int32 ColorMapIndex;

		Float4 Normal;
		int32 NormalMapIndex;
	};

	class HYPER_API RayTracingPipeline : public ScriptablePipeline
	{
	private:
		struct RTFrameData
		{
			Matrix4 View;
			Matrix4 Perspective;
			Matrix4 ViewInv;
			Matrix4 PerspectiveInv;
			Float3 ViewPosition;
		};

		nvrhi::SamplerHandle m_LinearSampler;
		nvrhi::BufferHandle m_FrameConstantBuffer;
		nvrhi::BufferHandle m_InstanceInfoBuffer;
		nvrhi::BufferHandle m_MaterialBuffer;

		nvrhi::BindingLayoutHandle m_Layout;
		nvrhi::BindingLayoutHandle m_BindlessLayout;
		DescriptorTableManager* m_BindlessManager;

		nvrhi::rt::AccelStructHandle m_TLAS;
		nvrhi::rt::PipelineHandle m_Pipeline;
		nvrhi::rt::ShaderTableHandle m_ShaderTable;
		nvrhi::TextureHandle m_RenderTarget;

		std::vector<nvrhi::rt::InstanceDesc> m_RTInstances;
		std::vector<InstanceBufferIndices> m_InstanceBufferIndices;
		std::vector<RTMaterial> m_RTMaterials;

		void prepareBindlessResources(nvrhi::DeviceHandle device);
		void createRayTracingPipeline(nvrhi::DeviceHandle device);
		void buildInstances(const ScriptablePipelineRenderingContext& context);
		void buildAccelerationStructure(nvrhi::CommandListHandle cmd, nvrhi::DeviceHandle device);
		void dispatchRays(ScriptablePipelineRenderingContext& context, Camera& camera);

	public:
		RayTracingPipeline();
		~RayTracingPipeline() = default;

		void Render(ScriptablePipelineRenderingContext& context, Camera& camera);
	};
}
