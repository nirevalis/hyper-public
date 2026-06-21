#include "HybridStyledPipeline.hpp"

#include <Windows.Storage.FileProperties.h>
#include <Core/Math/Matrix3.hpp>

#include "Core/Math/Matrix4.hpp"
#include "Engine/GameEngine.hpp"
#include "Renderer/DescriptorsCache.hpp"
#include "Renderer/ShaderLibrary.hpp"
#include <Renderer/Mesh.hpp>

#include "Asset/AssetRegistry.hpp"
#include "nvrhi/utils.h"
#include "Renderer/FramebufferCache.hpp"
#include "Renderer/PSOCache.hpp"
#include "Renderer/Assets/TextureAsset.hpp"

namespace Hyper
{
	namespace
	{
		nvrhi::SamplerHandle BilinearSampler;

		nvrhi::InputLayoutHandle GlobalVertexLayout;
		nvrhi::BindingLayoutHandle GlobalLayout;
	}

	struct VertexFullscreen
	{
		Float2 Position;
		Float2 TexCoord;

		VertexFullscreen()
		{

		}

		VertexFullscreen(const Float2& position, const Float2& texCoord) : Position(position), TexCoord(texCoord)
		{

		}
	};

	struct ProceduralSkyShaderParameters
	{
		Matrix4 InvView;

		Float3 DirectionToLight;
		float AngularSizeOfLight;

		Float3 LightColor;
		float GlowSize;

		Float3 SkyColor;
		float GlowIntensity;

		Float3 HorizonColor;
		float HorizonSize;

		Float3 GroundColor;
		float GlowSharpness;

		Float3 DirectionUp;
		float pad1;
	};

	struct RayTraceShadowsConstants
	{
		Float3 SunDirection;
		float pad0;
	};

	struct RayTraceAOConstants
	{
		float Radius;
		float Power;
		float Bias;
	};

	struct LightPassConstants
	{
		Float3 CameraPosition;
		float AOIntensity;
	};

	void HybridStyledPipeline::prepareGraphicsBuffers(ScriptablePipelineRenderingContext& context)
	{
		Float2 bufferSize = Float2::Zero;
		if (m_GBuffer.Position)
			bufferSize = {
				static_cast< float >(m_GBuffer.Position.Get()->getDesc().width) ,
				static_cast< float >(m_GBuffer.Position.Get()->getDesc().height) ,
		};


		if (context.Viewport != bufferSize)
		{
			m_GBuffer.Position = nullptr;
			m_GBuffer.Normal = nullptr;
			m_GBuffer.Color = nullptr;
			m_GBuffer.Depth = nullptr;

			nvrhi::TextureDesc desc;
			desc.width = context.Viewport.X;
			desc.height = context.Viewport.Y;
			desc.initialState = nvrhi::ResourceStates::RenderTarget;
			desc.isRenderTarget = true;
			desc.useClearValue = true;
			desc.clearValue = nvrhi::Color(0.f);
			desc.dimension = nvrhi::TextureDimension::Texture2D;
			desc.keepInitialState = true;
			desc.isTypeless = false;
			desc.isUAV = false;
			desc.mipLevels = 1;

			desc.format = nvrhi::Format::RGBA32_FLOAT;
			desc.debugName = "GBuffer Position";
			m_GBuffer.Position = context.Device->createTexture(desc);

			desc.format = nvrhi::Format::RGBA32_FLOAT;
			desc.debugName = "GBuffer Normal";
			m_GBuffer.Normal = context.Device->createTexture(desc);

			desc.format = nvrhi::Format::RGBA8_UNORM;
			desc.debugName = "GBuffer Color";
			m_GBuffer.Color = context.Device->createTexture(desc);

			const nvrhi::Format depthFormats[] =
			{
				nvrhi::Format::D24S8,
				nvrhi::Format::D32S8,
				nvrhi::Format::D32,
				nvrhi::Format::D16
			};

			const nvrhi::FormatSupport depthFeatures = nvrhi::FormatSupport::Texture | nvrhi::FormatSupport::DepthStencil | nvrhi::FormatSupport::ShaderLoad;

			desc.format = nvrhi::utils::ChooseFormat(context.Device, depthFeatures, depthFormats, std::size(depthFormats));
			desc.isTypeless = true;
			desc.initialState = nvrhi::ResourceStates::DepthWrite;
			desc.clearValue = nvrhi::Color(1, 0, 0, 0);
			desc.debugName = "GBuffer Depth";

			m_GBuffer.Depth = context.Device->createTexture(desc);
		}

	}

	void HybridStyledPipeline::createPipelineForSurface(SurfaceType type, const String& psShader)
	{
		auto desc = nvrhi::GraphicsPipelineDesc();
		desc.setInputLayout(GlobalVertexLayout)
			.renderState.rasterState.setCullNone();
		desc.renderState.depthStencilState.depthTestEnable = true;
		desc.renderState.depthStencilState.depthWriteEnable = true;

		{
			ShaderInformation quadShaderVS {  };
			quadShaderVS.Name = "RasterTesting";
			quadShaderVS.ShaderType = nvrhi::ShaderType::Vertex;
			desc.setVertexShader(ShaderLibrary::GetShader(quadShaderVS));
		}

		{
			ShaderInformation quadShaderPS {  };
			quadShaderPS.Name = psShader;
			quadShaderPS.ShaderType = nvrhi::ShaderType::Pixel;
			desc.setPixelShader(ShaderLibrary::GetShader(quadShaderPS));
		}


		desc.addBindingLayout(GlobalLayout);

		m_GBuffer.m_Pipelines[type] = desc;
	}

	HybridStyledPipeline::HybridStyledPipeline()
	{
		nvrhi::DeviceHandle device = GameEngine::Get().GetDeviceHelper().GetDevice();

		nvrhi::SamplerDesc samplerDesc;
		samplerDesc.setMagFilter(true);
		samplerDesc.setMipFilter(true);
		BilinearSampler = device->createSampler(samplerDesc);

		GlobalLayout = device->createBindingLayout(nvrhi::BindingLayoutDesc()
			.setVisibility(nvrhi::ShaderType::All)
			.addItem(nvrhi::BindingLayoutItem::VolatileConstantBuffer(0))
			.addItem(nvrhi::BindingLayoutItem::StructuredBuffer_SRV(1)));

		nvrhi::VertexAttributeDesc attributes[] = {
			nvrhi::VertexAttributeDesc()
				.setName("POSITION")
				.setFormat(nvrhi::Format::RGB32_FLOAT)
				.setOffset(offsetof(Vertex3D, Position))
				.setElementStride(sizeof(Vertex3D)),
			nvrhi::VertexAttributeDesc()
				.setName("TEXCOORD")
				.setFormat(nvrhi::Format::RG32_FLOAT)
				.setOffset(offsetof(Vertex3D, TexCoord))
				.setElementStride(sizeof(Vertex3D)),
			nvrhi::VertexAttributeDesc()
				.setName("NORMAL")
				.setFormat(nvrhi::Format::RGB32_FLOAT)
				.setOffset(offsetof(Vertex3D, Normal))
				.setElementStride(sizeof(Vertex3D)),
		};

		GlobalVertexLayout = device->createInputLayout(attributes, 3u, nullptr);

		createPipelineForSurface(SurfaceType::Solid, "GBuffer.Solid");

		prepareFullscreenObjects(device);
		prepareRTObjects(device);

		{
			m_LightPass.Set = setupFullscreenPSO(device, "LightPass", nvrhi::BindingLayoutDesc()
				.setVisibility(nvrhi::ShaderType::All)
				.addItem(nvrhi::BindingLayoutItem::VolatileConstantBuffer(0))
				.addItem(nvrhi::BindingLayoutItem::Texture_SRV(0))
				.addItem(nvrhi::BindingLayoutItem::Texture_SRV(1))
				.addItem(nvrhi::BindingLayoutItem::Texture_SRV(2))
				.addItem(nvrhi::BindingLayoutItem::Texture_SRV(3))
				.addItem(nvrhi::BindingLayoutItem::Texture_SRV(4))
				.addItem(nvrhi::BindingLayoutItem::Sampler(0)), &m_LightPass.PSO);

			m_LightPass.CBV = device->createBuffer(nvrhi::utils::CreateVolatileConstantBufferDesc(sizeof(LightPassConstants), "LightPass CBV", 16));
		}

		{
			m_FXAAPass.Set = setupFullscreenPSO(device, "FXAA", nvrhi::BindingLayoutDesc()
				.setVisibility(nvrhi::ShaderType::All)
				.addItem(nvrhi::BindingLayoutItem::Texture_SRV(0))
				.addItem(nvrhi::BindingLayoutItem::Sampler(0)), &m_FXAAPass.PSO);
		}

		{
			auto desc = nvrhi::GraphicsPipelineDesc();
			desc.renderState.rasterState.setCullNone();
			desc.renderState.depthStencilState.depthTestEnable = true;
			desc.renderState.depthStencilState.depthWriteEnable = false;
			desc.renderState.depthStencilState.depthFunc = nvrhi::ComparisonFunc::LessOrEqual;

			{
				ShaderInformation quadShaderVS {  };
				quadShaderVS.Name = "Sky";
				quadShaderVS.ShaderType = nvrhi::ShaderType::Vertex;
				desc.setVertexShader(ShaderLibrary::GetShader(quadShaderVS));
			}

			{
				ShaderInformation quadShaderPS {  };
				quadShaderPS.Name = "Sky";
				quadShaderPS.ShaderType = nvrhi::ShaderType::Pixel;
				desc.setPixelShader(ShaderLibrary::GetShader(quadShaderPS));
			}

			m_SkyPass.Set = device->createBindingLayout(nvrhi::BindingLayoutDesc()
				.setVisibility(nvrhi::ShaderType::All)
				.addItem(nvrhi::BindingLayoutItem::PushConstants(0, sizeof(Matrix4) * 2))
				.addItem(nvrhi::BindingLayoutItem::Texture_SRV(0))
				.addItem(nvrhi::BindingLayoutItem::Texture_SRV(1))
				.addItem(nvrhi::BindingLayoutItem::Sampler(0))
				//.addItem(nvrhi::BindingLayoutItem::Texture_SRV(2))
			);

			desc.addBindingLayout(m_SkyPass.Set);

			m_SkyPass.PSO = desc;
		}
	}

	void HybridStyledPipeline::prepareRTObjects(nvrhi::DeviceHandle device)
	{
		RayTracingInfo& rt = m_RayTracingInfo;

		nvrhi::BindlessLayoutDesc bindlessLayoutDesc;
		bindlessLayoutDesc
			.setVisibility(nvrhi::ShaderType::All)
			.setFirstSlot(0)
			.setMaxCapacity(8192)
			.addRegisterSpace(nvrhi::BindingLayoutItem::RawBuffer_SRV(1));

		rt.BindlessLayout = device->createBindlessLayout(bindlessLayoutDesc);
		rt.BindlessManager = new DescriptorTableManager(device, rt.BindlessLayout);

		{
			RTShadows& shadows = m_RTShadows;
			//TODO: for now we dont need access to vertex / index buffers
			// This is only setup for shadow raytracing. Soon we will add more.
			auto globalRTBindings = nvrhi::BindingLayoutDesc()
				.setVisibility(nvrhi::ShaderType::All)
				.addItem(nvrhi::BindingLayoutItem::PushConstants(0, sizeof(RayTraceShadowsConstants)))
				.addItem(nvrhi::BindingLayoutItem::Texture_UAV(0))
				.addItem(nvrhi::BindingLayoutItem::RayTracingAccelStruct(0))
				.addItem(nvrhi::BindingLayoutItem::Texture_SRV(1))  // Position
				.addItem(nvrhi::BindingLayoutItem::Texture_SRV(2))  // Normal
				.addItem(nvrhi::BindingLayoutItem::Texture_SRV(3))  // Color
				.addItem(nvrhi::BindingLayoutItem::Texture_SRV(4)); // Depth

			shadows.Set = device->createBindingLayout(globalRTBindings);

			nvrhi::rt::PipelineDesc pipelineDesc;
			pipelineDesc.globalBindingLayouts = { shadows.Set };

			{
				ShaderInformation gen {  };
				gen.Name = "RayTracing.Shadow";
				gen.ShaderType = nvrhi::ShaderType::RayGeneration;
				pipelineDesc.addShader(nvrhi::rt::PipelineShaderDesc("", ShaderLibrary::GetShader(gen), nullptr));
			}

			{
				ShaderInformation gen {  };
				gen.Name = "RayTracing.Shadow";
				gen.ShaderType = nvrhi::ShaderType::Miss;
				pipelineDesc.addShader(nvrhi::rt::PipelineShaderDesc("", ShaderLibrary::GetShader(gen), nullptr));
			}

			nvrhi::ShaderHandle closestHit;
			{
				ShaderInformation gen {  };
				gen.Name = "RayTracing.Shadow";
				gen.ShaderType = nvrhi::ShaderType::ClosestHit;
				closestHit = ShaderLibrary::GetShader(gen);
			}

			pipelineDesc.addHitGroup({ "CL_Main", closestHit, nullptr,nullptr,nullptr,false });
			pipelineDesc.maxPayloadSize = sizeof(int32) + sizeof(float);

			shadows.PSO = device->createRayTracingPipeline(pipelineDesc);

			shadows.SBT = shadows.PSO->createShaderTable();
			shadows.SBT->setRayGenerationShader("RG_Main");
			shadows.SBT->addHitGroup("CL_Main");
			shadows.SBT->addMissShader("MI_Main");
		}

		{
			RTAO& ao = m_RTAO;
			//TODO: for now we dont need access to vertex / index buffers
			// This is only setup for shadow raytracing. Soon we will add more.
			auto globalRTBindings = nvrhi::BindingLayoutDesc()
				.setVisibility(nvrhi::ShaderType::All)
				.addItem(nvrhi::BindingLayoutItem::PushConstants(0, sizeof(RayTraceAOConstants)))
				.addItem(nvrhi::BindingLayoutItem::Texture_UAV(0))
				.addItem(nvrhi::BindingLayoutItem::RayTracingAccelStruct(0))
				.addItem(nvrhi::BindingLayoutItem::Texture_SRV(1))  // Position
				.addItem(nvrhi::BindingLayoutItem::Texture_SRV(2))  // Normal
				.addItem(nvrhi::BindingLayoutItem::Texture_SRV(3))  // Color
				.addItem(nvrhi::BindingLayoutItem::Texture_SRV(4)); // Depth

			ao.Set = device->createBindingLayout(globalRTBindings);

			nvrhi::rt::PipelineDesc pipelineDesc;
			pipelineDesc.globalBindingLayouts = { ao.Set };

			{
				ShaderInformation gen {  };
				gen.Name = "RayTracing.AO";
				gen.ShaderType = nvrhi::ShaderType::RayGeneration;
				pipelineDesc.addShader(nvrhi::rt::PipelineShaderDesc("", ShaderLibrary::GetShader(gen), nullptr));
			}

			{
				ShaderInformation gen {  };
				gen.Name = "RayTracing.AO";
				gen.ShaderType = nvrhi::ShaderType::Miss;
				pipelineDesc.addShader(nvrhi::rt::PipelineShaderDesc("", ShaderLibrary::GetShader(gen), nullptr));
			}

			nvrhi::ShaderHandle closestHit;
			{
				ShaderInformation gen {  };
				gen.Name = "RayTracing.AO";
				gen.ShaderType = nvrhi::ShaderType::ClosestHit;
				closestHit = ShaderLibrary::GetShader(gen);
			}

			pipelineDesc.addHitGroup({ "CL_Main", closestHit, nullptr,nullptr,nullptr,false });
			pipelineDesc.maxPayloadSize = sizeof(int32) + sizeof(float);

			ao.PSO = device->createRayTracingPipeline(pipelineDesc);

			ao.SBT = ao.PSO->createShaderTable();
			ao.SBT->setRayGenerationShader("RG_Main");
			ao.SBT->addHitGroup("CL_Main");
			ao.SBT->addMissShader("MI_Main");
		}
	}

	void HybridStyledPipeline::prepareFullscreenObjects(nvrhi::DeviceHandle device)
	{
		std::vector<VertexFullscreen> vertices = {
			VertexFullscreen(Float2(-1.0f,  1.0f), Float2(0.0f, 0.0f)),
			VertexFullscreen(Float2(-1.0f, -1.0f), Float2(0.0f, 1.0f)),
			VertexFullscreen(Float2(1.0f,  1.0f), Float2(1.0f, 0.0f)), // Top-Right
			VertexFullscreen(Float2(1.0f, -1.0f), Float2(1.0f, 1.0f)), // Bottom-Right
			VertexFullscreen(Float2(-1.0f, -1.0f), Float2(0.0f, 1.0f)), // Bottom-Left (reused)
			VertexFullscreen(Float2(1.0f,  1.0f), Float2(1.0f, 0.0f)), // Top-Right (reused)
		};

		auto vertexBufferDesc = nvrhi::BufferDesc()
			.setByteSize(vertices.size() * sizeof(VertexFullscreen))
			.setIsVertexBuffer(true)
			.setInitialState(nvrhi::ResourceStates::VertexBuffer)
			.setKeepInitialState(true)
			.setDebugName("Fullscreen Vertex Buffer");

		m_FullScreenVB = device->createBuffer(vertexBufferDesc);

		GameEngine::Get().GetDeviceHelper().Submit([=] (nvrhi::CommandListHandle cmd)
			{
				cmd->writeBuffer(m_FullScreenVB, vertices.data(), vertices.size() * sizeof(VertexFullscreen));
			});

		nvrhi::VertexAttributeDesc attributes[] = {
			nvrhi::VertexAttributeDesc()
				.setName("POSITION")
				.setFormat(nvrhi::Format::RG32_FLOAT)
				.setOffset(offsetof(VertexFullscreen, Position))
				.setElementStride(sizeof(VertexFullscreen)),
			nvrhi::VertexAttributeDesc()
				.setName("TEXCOORD")
				.setFormat(nvrhi::Format::RG32_FLOAT)
				.setOffset(offsetof(VertexFullscreen, TexCoord))
				.setElementStride(sizeof(VertexFullscreen))
		};

		m_FullScreenIL = device->createInputLayout(attributes, 0u, nullptr);
	}

	nvrhi::BindingLayoutHandle HybridStyledPipeline::setupFullscreenPSO(nvrhi::DeviceHandle device, const String& shaderName, const nvrhi::BindingLayoutDesc& layoutDesc, nvrhi::GraphicsPipelineDesc* outPso)
	{
		auto psoDesc = nvrhi::GraphicsPipelineDesc();
		psoDesc.setInputLayout(m_FullScreenIL);
		psoDesc.renderState.rasterState.setCullNone();

		psoDesc.renderState.depthStencilState.depthTestEnable = false;
		psoDesc.renderState.depthStencilState.depthWriteEnable = false;

		{
			ShaderInformation quadShaderVS {  };
			quadShaderVS.Name = shaderName;
			quadShaderVS.ShaderType = nvrhi::ShaderType::Vertex;
			psoDesc.setVertexShader(ShaderLibrary::GetShader(quadShaderVS));
		}

		{
			ShaderInformation quadShaderPS {  };
			quadShaderPS.Name = shaderName;
			quadShaderPS.ShaderType = nvrhi::ShaderType::Pixel;
			psoDesc.setPixelShader(ShaderLibrary::GetShader(quadShaderPS));
		}

		nvrhi::BindingLayoutHandle layout = device->createBindingLayout(layoutDesc);
		psoDesc.addBindingLayout(layout);

		*outPso = psoDesc;

		return layout;
	}

	HybridStyledPipeline::GPUMaterial HybridStyledPipeline::buildMaterial(const Material& material)
	{
		GPUMaterial gMaterial;

		gMaterial.Color = material.Color.Value;
		gMaterial.Normal = material.Normal.Value;
		gMaterial.Roughness = material.Roughness;
		gMaterial.Metallic = material.Metallic;

		return gMaterial;
	}

	MeshCBV HybridStyledPipeline::getPerObjectData(const RadiansTransform& t)
	{
		MeshCBV matrices;

		Matrix4 rotation = Matrix4::FromQuaternion(t.Orientation);
		Matrix3 model = Matrix3::Identity();

		for (int r = 0; r < 3; ++r)
		{
			model.Values[r][0] = rotation.Values[r][0] * t.Scale.X;
			model.Values[r][1] = rotation.Values[r][1] * t.Scale.Y;
			model.Values[r][2] = rotation.Values[r][2] * t.Scale.Z;
		}

		Matrix3 normalMatrix = model.Inverse().Transpose();

		for (int r = 0; r < 3; ++r)
		{
			matrices.Transform[r * 4 + 0] = model.Values[r][0];
			matrices.Transform[r * 4 + 1] = model.Values[r][1];
			matrices.Transform[r * 4 + 2] = model.Values[r][2];
			matrices.Transform[r * 4 + 3] = t.Position.Raw[r];

			matrices.NormalMatrix[r][0] = normalMatrix.Values[r][0];
			matrices.NormalMatrix[r][1] = normalMatrix.Values[r][1];
			matrices.NormalMatrix[r][2] = normalMatrix.Values[r][2];
			matrices.NormalMatrix[r][3] = 0.0f;
		}

		matrices.View = m_View * m_Projection;

		return matrices;
	}

	void HybridStyledPipeline::prepareCBVPool(ScriptablePipelineRenderingContext& context)
	{
		if (m_MeshCBVPool.size() < context.DrawCalls.size())
		{
			int32 delta = context.DrawCalls.size() - m_MeshCBVPool.size();
			for (int32 i = 0; i < delta; i++)
				m_MeshCBVPool.push(context.Device->createBuffer(nvrhi::utils::CreateVolatileConstantBufferDesc(sizeof(MeshCBV), "Mesh CBV Pool's Buffer", 16)));
		}
	}

	void HybridStyledPipeline::fillGraphicsBuffer(ScriptablePipelineRenderingContext& context)
	{
		nvrhi::DeviceHandle device = context.Device;
		nvrhi::CommandListHandle cmd = context.Context;

		cmd->beginMarker("GBuffer Pass");

		prepareCBVPool(context);

		nvrhi::FramebufferDesc bufferDesc;
		bufferDesc.addColorAttachment(m_GBuffer.Position);
		bufferDesc.addColorAttachment(m_GBuffer.Normal);
		bufferDesc.addColorAttachment(m_GBuffer.Color);
		bufferDesc.setDepthAttachment(m_GBuffer.Depth);

		nvrhi::FramebufferHandle fb = FramebufferCache::Get(device, bufferDesc);

		nvrhi::utils::ClearColorAttachment(cmd, fb, 0, nvrhi::Color(0, 0, 0, 0));
		nvrhi::utils::ClearColorAttachment(cmd, fb, 1, nvrhi::Color(0, 0, 0, 0));
		nvrhi::utils::ClearColorAttachment(cmd, fb, 2, nvrhi::Color(0, 0, 0, 0));

		cmd->clearDepthStencilTexture(m_GBuffer.Depth, nvrhi::AllSubresources, true, 1, true, 0);

		m_GBuffer.m_MaterialList.clear();
		m_GBuffer.m_MaterialList.reserve(context.DrawCalls.size());

		for (const DrawCall& call : context.DrawCalls)
			m_GBuffer.m_MaterialList.push_back(buildMaterial(call.Material));

		if (!m_GBuffer.m_MaterialBuffer || m_GBuffer.m_MaterialBuffer->getDesc().byteSize != m_GBuffer.m_MaterialList.size() * sizeof(GPUMaterial))
		{
			m_GBuffer.m_MaterialBuffer = nullptr;

			auto materialBufferDesc = nvrhi::BufferDesc();
			materialBufferDesc.setStructStride(sizeof(GPUMaterial));
			materialBufferDesc.setByteSize(sizeof(GPUMaterial) * Math::Clamp(( int32 )m_GBuffer.m_MaterialList.size(), 1, INT32_MAX));
			materialBufferDesc.debugName = "GBuffer Materials";
			materialBufferDesc.isVolatile = false;
			materialBufferDesc.canHaveUAVs = false;
			materialBufferDesc.initialState = nvrhi::ResourceStates::ShaderResource;
			materialBufferDesc.keepInitialState = true;
			m_GBuffer.m_MaterialBuffer = device->createBuffer(materialBufferDesc);
		}

		cmd->writeBuffer(m_GBuffer.m_MaterialBuffer, m_GBuffer.m_MaterialList.data(), m_GBuffer.m_MaterialList.size() * sizeof(GPUMaterial));

		nvrhi::GraphicsState state;
		state.setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport { 0,context.Viewport.X,0, context.Viewport.Y, 0,1 }));
		state.setFramebuffer(fb);

		uint32 index = 0;
		for (const DrawCall& call : context.DrawCalls)
		{
			auto cbv = m_MeshCBVPool.front(); m_MeshCBVPool.pop();

			auto set = DescriptorsCache::Get(device, GlobalLayout, nvrhi::BindingSetDesc()
				.addItem(nvrhi::BindingSetItem::ConstantBuffer(0, cbv))
				.addItem(nvrhi::BindingSetItem::StructuredBuffer_SRV(1, m_GBuffer.m_MaterialBuffer)));


			auto perObject = getPerObjectData(call.Transform);
			perObject.Material = index++;

			cmd->writeBuffer(cbv, &perObject, sizeof(perObject));

			nvrhi::GraphicsState callState = state;
			callState.setPipeline(PSOCache::Get(device, fb, m_GBuffer.m_Pipelines[call.Material.Type]));
			callState.addVertexBuffer({ call.Mesh->VertexBuffer });
			callState.setIndexBuffer(nvrhi::IndexBufferBinding(call.Mesh->IndexBuffer).setFormat(nvrhi::Format::R32_UINT));
			callState.addBindingSet(set);

			cmd->setGraphicsState(callState);

			auto drawArguments = nvrhi::DrawArguments();
			drawArguments.setVertexCount(call.Mesh->IndexBuffer.Get()->getDesc().byteSize / sizeof(uint32));
			cmd->drawIndexed(drawArguments);

			m_MeshCBVPool.push(cbv);
		}

		cmd->endMarker();
	}

	void HybridStyledPipeline::recreateAS(ScriptablePipelineRenderingContext& context)
	{
		RayTracingInfo& rt = m_RayTracingInfo;

		context.Context->beginMarker("Bake Acceleration Structures");

		rt.RTInstances.clear();
		rt.RTInstances.reserve(context.DrawCalls.size());

		int32 instanceId = 0;
		for (const DrawCall& call : context.DrawCalls)
		{
			auto instanceDesc = nvrhi::rt::InstanceDesc()
				.setBLAS(call.Mesh->GetBLAS())
				.setFlags(nvrhi::rt::InstanceFlags::TriangleCullDisable)
				.setInstanceMask(1)
				.setInstanceID(instanceId++)
				.setTransform(getPerObjectData(call.Transform).Transform);

			rt.RTInstances.push_back(instanceDesc);
		}

		if (!rt.TLAS || rt.TLAS->getDesc().topLevelMaxInstances != context.DrawCalls.size())
		{
			rt.TLAS = nullptr;

			auto tlasDesc = nvrhi::rt::AccelStructDesc()
				.setDebugName("RayTracing TLAS")
				.setIsTopLevel(true)
				.setBuildFlags(nvrhi::rt::AccelStructBuildFlags::AllowUpdate)
				.setTopLevelMaxInstances(context.DrawCalls.size());

			rt.TLAS = context.Device->createAccelStruct(tlasDesc);

			context.Context->buildTopLevelAccelStruct(rt.TLAS, rt.RTInstances.data(), rt.RTInstances.size());
		}
		else
			context.Context->buildTopLevelAccelStruct(rt.TLAS, rt.RTInstances.data(), rt.RTInstances.size(), nvrhi::rt::AccelStructBuildFlags::AllowUpdate | nvrhi::rt::AccelStructBuildFlags::PerformUpdate);

		context.Context->endMarker();
	}

	void HybridStyledPipeline::rayTracedShadowPass(ScriptablePipelineRenderingContext& context, Camera& camera)
	{
		RayTracingInfo& rt = m_RayTracingInfo;
		RTShadows& shadows = m_RTShadows;

		context.Context->beginMarker("Raytraced Shadows Pass");

		if (!shadows.Output || shadows.Output.Get()->getDesc().width != context.Viewport.X || shadows.Output.Get()->getDesc().height != context.Viewport.Y)
		{
			shadows.Output = nullptr;

			nvrhi::TextureDesc desc;
			desc.width = context.Viewport.X;
			desc.height = context.Viewport.Y;
			desc.initialState = nvrhi::ResourceStates::UnorderedAccess;
			desc.isRenderTarget = false;
			desc.dimension = nvrhi::TextureDimension::Texture2D;
			desc.keepInitialState = true;
			desc.isTypeless = false;
			desc.isUAV = true;
			desc.mipLevels = 1;

			desc.format = nvrhi::Format::R8_UNORM;
			desc.debugName = "RayTrace Shadow RT";
			shadows.Output = context.Device->createTexture(desc);
		}

		nvrhi::BindingSetDesc setDesc {};
		setDesc.addItem(nvrhi::BindingSetItem::PushConstants(0, sizeof(RayTraceShadowsConstants)));
		setDesc.addItem(nvrhi::BindingSetItem::Texture_UAV(0, shadows.Output));
		setDesc.addItem(nvrhi::BindingSetItem::RayTracingAccelStruct(0, rt.TLAS))
			.addItem(nvrhi::BindingSetItem::Texture_SRV(1, m_GBuffer.Position))
			.addItem(nvrhi::BindingSetItem::Texture_SRV(2, m_GBuffer.Normal))
			.addItem(nvrhi::BindingSetItem::Texture_SRV(3, m_GBuffer.Color))
			.addItem(nvrhi::BindingSetItem::Texture_SRV(4, m_GBuffer.Depth));

		nvrhi::rt::State state;
		state.setShaderTable(shadows.SBT);
		state.addBindingSet(DescriptorsCache::Get(context.Device, shadows.Set, setDesc));
		//state.addBindingSet(rt.BindlessManager->GetDescriptorTable());

		context.Context->setRayTracingState(state);

		RayTraceShadowsConstants data;
		data.SunDirection = Float3(-2, -1, -3.2f);
		data.pad0 = 0.0f;

		context.Context->setPushConstants(&data, sizeof(data));

		nvrhi::rt::DispatchRaysArguments args;
		args.width = context.Viewport.X;
		args.height = context.Viewport.Y;
		context.Context->dispatchRays(args);

		context.Context->endMarker();
	}

	void HybridStyledPipeline::rayTracedAO(ScriptablePipelineRenderingContext& context, Camera& camera)
	{
		RayTracingInfo& rt = m_RayTracingInfo;
		RTAO& ao = m_RTAO;

		context.Context->beginMarker("Raytraced AO Pass");

		if (!ao.Output || ao.Output.Get()->getDesc().width != context.Viewport.X || ao.Output.Get()->getDesc().height != context.Viewport.Y)
		{
			ao.Output = nullptr;

			nvrhi::TextureDesc desc;
			desc.width = context.Viewport.X;
			desc.height = context.Viewport.Y;
			desc.initialState = nvrhi::ResourceStates::UnorderedAccess;
			desc.isRenderTarget = false;
			desc.dimension = nvrhi::TextureDimension::Texture2D;
			desc.keepInitialState = true;
			desc.isTypeless = false;
			desc.isUAV = true;
			desc.mipLevels = 1;

			desc.format = nvrhi::Format::RGBA8_UNORM;
			desc.debugName = "RayTrace AO RT";
			ao.Output = context.Device->createTexture(desc);
		}

		nvrhi::BindingSetDesc setDesc {};
		setDesc.addItem(nvrhi::BindingSetItem::PushConstants(0, sizeof(RayTraceAOConstants)));
		setDesc.addItem(nvrhi::BindingSetItem::Texture_UAV(0, ao.Output));
		setDesc.addItem(nvrhi::BindingSetItem::RayTracingAccelStruct(0, rt.TLAS))
			.addItem(nvrhi::BindingSetItem::Texture_SRV(1, m_GBuffer.Position))
			.addItem(nvrhi::BindingSetItem::Texture_SRV(2, m_GBuffer.Normal))
			.addItem(nvrhi::BindingSetItem::Texture_SRV(3, m_GBuffer.Color))
			.addItem(nvrhi::BindingSetItem::Texture_SRV(4, m_GBuffer.Depth));

		nvrhi::rt::State state;
		state.setShaderTable(ao.SBT);
		state.addBindingSet(DescriptorsCache::Get(context.Device, ao.Set, setDesc));
		//state.addBindingSet(rt.BindlessManager->GetDescriptorTable());

		context.Context->setRayTracingState(state);

		RayTraceAOConstants data;
		data.Radius = 0.45f;
		data.Power  = 1.1f;
		data.Bias   = 0.06f;

		context.Context->setPushConstants(&data, sizeof(data));

		nvrhi::rt::DispatchRaysArguments args;
		args.width = context.Viewport.X;
		args.height = context.Viewport.Y;
		context.Context->dispatchRays(args);

		context.Context->endMarker();
	}

	void HybridStyledPipeline::lightPass(ScriptablePipelineRenderingContext& context, Camera& camera)
	{
		context.Context->beginMarker("Light Pass");

		if (!m_LightPass.Output || m_LightPass.Output.Get()->getDesc().width != context.Viewport.X || m_LightPass.Output.Get()->getDesc().height != context.Viewport.Y)
		{
			m_LightPass.Output = nullptr;

			nvrhi::TextureDesc desc;
			desc.width = context.Viewport.X;
			desc.height = context.Viewport.Y;
			desc.initialState = nvrhi::ResourceStates::RenderTarget;
			desc.isRenderTarget = true;
			desc.useClearValue = true;
			desc.clearValue = nvrhi::Color(0.f);
			desc.dimension = nvrhi::TextureDimension::Texture2D;
			desc.keepInitialState = true;
			desc.isTypeless = false;
			desc.isUAV = false;
			desc.mipLevels = 1;

			desc.format = nvrhi::Format::RGBA8_UNORM;
			desc.debugName = "LightPass RT";
			m_LightPass.Output = context.Device->createTexture(desc);
		}

		nvrhi::FramebufferDesc bufferDesc;
		bufferDesc.addColorAttachment(m_LightPass.Output);

		context.Context->clearTextureFloat(m_LightPass.Output, nvrhi::AllSubresources, nvrhi::Color { 0,0,0,0 });

		LightPassConstants cbv { camera.GetTransform().Position, 2.0f };
		context.Context->writeBuffer(m_LightPass.CBV, &cbv, sizeof(cbv));

		nvrhi::GraphicsState state;
		state.setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport { 0,context.Viewport.X,0, context.Viewport.Y, 0,1 }));
		state.setFramebuffer(FramebufferCache::Get(context.Device, bufferDesc));
		state.setPipeline(PSOCache::Get(context.Device, state.framebuffer, m_LightPass.PSO));
		state.addBindingSet(DescriptorsCache::Get(context.Device, m_LightPass.Set, nvrhi::BindingSetDesc()
			.addItem(nvrhi::BindingSetItem::ConstantBuffer(0, m_LightPass.CBV))
			.addItem(nvrhi::BindingSetItem::Texture_SRV(0, m_GBuffer.Position))
			.addItem(nvrhi::BindingSetItem::Texture_SRV(1, m_GBuffer.Normal))
			.addItem(nvrhi::BindingSetItem::Texture_SRV(2, m_GBuffer.Color))
			.addItem(nvrhi::BindingSetItem::Texture_SRV(3, m_RTShadows.Output))
			.addItem(nvrhi::BindingSetItem::Texture_SRV(4, AssetRegistry::GetAs<TextureAsset>("irradiance")->GetTexture()))
			//.addItem(nvrhi::BindingSetItem::Texture_SRV(5, m_RTAO.Output))
			.addItem(nvrhi::BindingSetItem::Sampler(0, BilinearSampler))
		));

		renderFullscreen(context.Context, state);

		context.Context->endMarker();
	}

	void HybridStyledPipeline::fxaaPass(ScriptablePipelineRenderingContext& context, Camera& camera)
	{
		context.Context->beginMarker("FXAA Pass");

		if (!m_FXAAPass.Output || m_FXAAPass.Output.Get()->getDesc().width != context.Viewport.X || m_FXAAPass.Output.Get()->getDesc().height != context.Viewport.Y)
		{
			m_FXAAPass.Output = nullptr;

			nvrhi::TextureDesc desc;
			desc.width = context.Viewport.X;
			desc.height = context.Viewport.Y;
			desc.initialState = nvrhi::ResourceStates::RenderTarget;
			desc.isRenderTarget = true;
			desc.useClearValue = true;
			desc.clearValue = nvrhi::Color(0.f);
			desc.dimension = nvrhi::TextureDimension::Texture2D;
			desc.keepInitialState = true;
			desc.isTypeless = false;
			desc.isUAV = false;
			desc.mipLevels = 1;

			desc.format = nvrhi::Format::RGBA8_UNORM;
			desc.debugName = "FXAA RT";
			m_FXAAPass.Output = context.Device->createTexture(desc);
		}

		nvrhi::FramebufferDesc bufferDesc;
		bufferDesc.addColorAttachment(m_FXAAPass.Output);

		context.Context->clearTextureFloat(m_FXAAPass.Output, nvrhi::AllSubresources, nvrhi::Color { 0,0,0,0 });

		nvrhi::GraphicsState state;
		state.setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport { 0,context.Viewport.X,0, context.Viewport.Y, 0,1 }));
		state.setFramebuffer(FramebufferCache::Get(context.Device, bufferDesc));
		state.setPipeline(PSOCache::Get(context.Device, state.framebuffer, m_FXAAPass.PSO));
		state.addBindingSet(DescriptorsCache::Get(context.Device, m_FXAAPass.Set, nvrhi::BindingSetDesc()
			.addItem(nvrhi::BindingSetItem::Texture_SRV(0, m_LightPass.Output))
			.addItem(nvrhi::BindingSetItem::Sampler(0, BilinearSampler))
		));

		renderFullscreen(context.Context, state);

		context.Context->endMarker();
	}


	void HybridStyledPipeline::skyPass(ScriptablePipelineRenderingContext& context, Camera& camera)
	{
		context.Context->beginMarker("Sky Pass");
		nvrhi::FramebufferDesc bufferDesc;
		bufferDesc.addColorAttachment(m_LightPass.Output);
		bufferDesc.setDepthAttachment(m_GBuffer.Depth);

		nvrhi::GraphicsState state;
		state.setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport { 0,context.Viewport.X,0, context.Viewport.Y, 0,1 }));
		state.setFramebuffer(FramebufferCache::Get(context.Device, bufferDesc));
		state.setPipeline(PSOCache::Get(context.Device, state.framebuffer, m_SkyPass.PSO));

		state.addBindingSet(DescriptorsCache::Get(context.Device, m_SkyPass.Set, nvrhi::BindingSetDesc()
			.addItem(nvrhi::BindingSetItem::PushConstants(0, sizeof(Matrix4) * 2))
			.addItem(nvrhi::BindingSetItem::Texture_SRV(0, AssetRegistry::GetAs<TextureAsset>("sky")->GetTexture()))
			.addItem(nvrhi::BindingSetItem::Texture_SRV(1, AssetRegistry::GetAs<TextureAsset>("irradiance")->GetTexture()))
			.addItem(nvrhi::BindingSetItem::Sampler(0, BilinearSampler))
			//.addItem(nvrhi::BindingSetItem::Texture_SRV(2, m_SkyPass.m_Skybox.GetIrradiance()))
		));

		std::vector matrices = {
			m_View,
			m_Projection
		};

		context.Context->setGraphicsState(state);
		context.Context->setPushConstants(matrices.data(), sizeof(Matrix4) * 2);

		auto drawArguments = nvrhi::DrawArguments();
		drawArguments.setVertexCount(36);
		context.Context->draw(drawArguments);

		context.Context->endMarker();
	}

	void HybridStyledPipeline::calculateMatrices(ScriptablePipelineRenderingContext& context, Camera& camera)
	{
		float yawSign = camera.GetTransform().GetUp().Y < 0 ? -1.0f : 1.0f;
		Float3 lookAt = camera.GetTransform().Position + camera.GetTransform().GetForward();

		m_View =
			Matrix4::LookAt(camera.GetTransform().Position, lookAt, Float3(0, yawSign, 0));
		m_Projection = Matrix4::Perspective(Math::Radians(90), context.Viewport.X / context.Viewport.Y, 0.01f, 1000.0f);
	}

	void HybridStyledPipeline::Render(ScriptablePipelineRenderingContext& context, Camera& camera)
	{
		nvrhi::DeviceHandle device = context.Device;
		nvrhi::CommandListHandle cmd = context.Context;

		//Prepare all resources
		prepareGraphicsBuffers(context);

		//Pre-calculate everything
		calculateMatrices(context, camera);

		//G-Buffer pass
		fillGraphicsBuffer(context);

		recreateAS(context);
		rayTracedShadowPass(context, camera);
		//rayTracedAO(context, camera);

		lightPass(context, camera);
		skyPass(context, camera);
		fxaaPass(context, camera);

		cmd->beginMarker("Composition");
		cmd->copyTexture(context.Output.Get()->getDesc().colorAttachments[0].texture, nvrhi::TextureSlice {}, m_FXAAPass.Output, nvrhi::TextureSlice {});
		cmd->endMarker();
	}
}
