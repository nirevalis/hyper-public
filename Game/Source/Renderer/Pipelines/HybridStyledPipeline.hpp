#pragma once

#include <queue>
#include <Core/Common.hpp>
#include <Core/Math/Matrix3.hpp>
#include <Renderer/ScriptablePipeline.hpp>
#include <nvrhi/nvrhi.h>
#include <Renderer/Skybox.hpp>
#include "Renderer/DescriptorTableManager.hpp"

namespace Hyper
{
	struct alignas(16) MeshCBV
	{
		float Transform[3 * 4];   // 48 bytes
		float NormalMatrix[3][4]; // padded float3x4 rows (matches float4 NormalMatrix[3])
		Matrix4 View;             // 64 bytes
		uint32_t Material;
		uint32_t _Padding0;
		uint32_t _Padding1[2];
	};




	class HYPER_API HybridStyledPipeline : public ScriptablePipeline
	{
	private:
		nvrhi::BufferHandle m_FullScreenVB;
		nvrhi::InputLayoutHandle m_FullScreenIL;

		std::queue<nvrhi::BufferHandle> m_MeshCBVPool;

		//TODO: implement textures when needed
		struct GPUMaterial
		{
			Float4 Color;
			Float4 Normal;
			float Roughness;
			float Metallic;
		};

		struct GBuffer
		{
			std::unordered_map<SurfaceType, nvrhi::GraphicsPipelineDesc> m_Pipelines;
			nvrhi::TextureHandle Position, Normal, Color, Depth;
			std::vector <GPUMaterial> m_MaterialList;
			nvrhi::BufferHandle m_MaterialBuffer;
		} m_GBuffer;

		Matrix4 m_Projection;
		Matrix4 m_View;

		struct LightPassInfo
		{
			nvrhi::TextureHandle Output;
			nvrhi::GraphicsPipelineDesc PSO;
			nvrhi::BindingLayoutHandle Set;
			nvrhi::BufferHandle CBV;
		} m_LightPass;

		struct FXAAInfo
		{
			nvrhi::TextureHandle Output;
			nvrhi::GraphicsPipelineDesc PSO;
			nvrhi::BindingLayoutHandle Set;
		} m_FXAAPass;

		struct SkyPassInfo
		{
			nvrhi::BindingLayoutHandle Set;
			nvrhi::GraphicsPipelineDesc PSO;
			//Skybox m_Skybox = Skybox("Skybox");
		} m_SkyPass;

		struct RayTracingInfo
		{
			nvrhi::BindingLayoutHandle BindlessLayout;
			DescriptorTableManager* BindlessManager;
			nvrhi::rt::AccelStructHandle TLAS;
			std::vector<nvrhi::rt::InstanceDesc> RTInstances;
		} m_RayTracingInfo;

		struct RTShadows
		{
			nvrhi::BindingLayoutHandle Set;
			nvrhi::rt::PipelineHandle PSO;
			nvrhi::rt::ShaderTableHandle SBT;
			nvrhi::TextureHandle Output;
		} m_RTShadows;

		struct RTAO
		{
			nvrhi::BindingLayoutHandle Set;
			nvrhi::rt::PipelineHandle PSO;
			nvrhi::rt::ShaderTableHandle SBT;
			nvrhi::TextureHandle Output;
		} m_RTAO;

		void prepareCBVPool(ScriptablePipelineRenderingContext& context);
		void prepareRTObjects(nvrhi::DeviceHandle device);
		void prepareFullscreenObjects(nvrhi::DeviceHandle device);
		void createPipelineForSurface(SurfaceType type, const String& psShader);


		template <typename T>
		inline void renderFullscreen(nvrhi::CommandListHandle cmd, nvrhi::GraphicsState state, const T& rootConstant)
		{
			nvrhi::GraphicsState callState = state;
			//callState.addVertexBuffer({ m_FullScreenVB });
			cmd->setGraphicsState(callState);
			cmd->setPushConstants(&rootConstant, sizeof(T));
			auto drawArguments = nvrhi::DrawArguments();
			drawArguments.setVertexCount(3);
			cmd->draw(drawArguments);
		}

		inline void renderFullscreen(nvrhi::CommandListHandle cmd, nvrhi::GraphicsState state)
		{
			nvrhi::GraphicsState callState = state;
			//callState.addVertexBuffer({ m_FullScreenVB });
			cmd->setGraphicsState(callState);

			auto drawArguments = nvrhi::DrawArguments();
			drawArguments.setVertexCount(3);
			cmd->draw(drawArguments);
		}

		nvrhi::BindingLayoutHandle setupFullscreenPSO(nvrhi::DeviceHandle device, const String& shaderName, const nvrhi::BindingLayoutDesc& layoutDesc, nvrhi::GraphicsPipelineDesc* outPso);

		static HybridStyledPipeline::GPUMaterial buildMaterial(const Material& material);
		MeshCBV getPerObjectData(const RadiansTransform& transform);

		void calculateMatrices(ScriptablePipelineRenderingContext& context, Camera& camera);
		void prepareGraphicsBuffers(ScriptablePipelineRenderingContext& context);
		void fillGraphicsBuffer(ScriptablePipelineRenderingContext& context);

		void recreateAS(ScriptablePipelineRenderingContext& context);

		void rayTracedShadowPass(ScriptablePipelineRenderingContext& context, Camera& camera);
		void rayTracedAO(ScriptablePipelineRenderingContext& context, Camera& camera);
		void lightPass(ScriptablePipelineRenderingContext& context, Camera& camera);
		void skyPass(ScriptablePipelineRenderingContext& context, Camera& camera);
		void fxaaPass(ScriptablePipelineRenderingContext& context, Camera& camera);
	public:
		HybridStyledPipeline();
		~HybridStyledPipeline() = default;

		void Render(ScriptablePipelineRenderingContext& context, Camera& camera);
	};
}
