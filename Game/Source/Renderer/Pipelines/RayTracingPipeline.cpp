#include "RayTracingPipeline.hpp"

#include "Core/Math/Matrix4.hpp"
#include "Engine/GameEngine.hpp"
#include "Renderer/DescriptorsCache.hpp"
#include "Renderer/ShaderLibrary.hpp"
#include <Renderer/Mesh.hpp>

#include "Asset/AssetRegistry.hpp"
#include "nvrhi/utils.h"

namespace Hyper
{
	void RayTracingPipeline::prepareBindlessResources(nvrhi::DeviceHandle device)
	{
		nvrhi::BindlessLayoutDesc bindlessLayoutDesc;
		bindlessLayoutDesc
			.setVisibility(nvrhi::ShaderType::All)
			.setFirstSlot(0)
			.setMaxCapacity(8192)
			.addRegisterSpace(nvrhi::BindingLayoutItem::RawBuffer_SRV(1))
			.addRegisterSpace(nvrhi::BindingLayoutItem::RawBuffer_SRV(2));

		m_BindlessLayout = device->createBindlessLayout(bindlessLayoutDesc);
		m_BindlessManager = new DescriptorTableManager(device, m_BindlessLayout);

		auto instanceInfoBufferDesc = nvrhi::BufferDesc();
		instanceInfoBufferDesc.setStructStride(sizeof(InstanceBufferIndices));
		instanceInfoBufferDesc.setByteSize(sizeof(InstanceBufferIndices) * 8192);
		instanceInfoBufferDesc.debugName = "InstanceInfos";
		instanceInfoBufferDesc.isVolatile = false;
		instanceInfoBufferDesc.canHaveUAVs = false;
		instanceInfoBufferDesc.initialState = nvrhi::ResourceStates::ShaderResource;
		instanceInfoBufferDesc.keepInitialState = true;
		m_InstanceInfoBuffer = device->createBuffer(instanceInfoBufferDesc);

		auto materialBufferDesc = nvrhi::BufferDesc();
		materialBufferDesc.setStructStride(sizeof(RTMaterial));
		materialBufferDesc.setByteSize(sizeof(RTMaterial) * 8192);
		materialBufferDesc.debugName = "MaterialInfos";
		materialBufferDesc.isVolatile = false;
		materialBufferDesc.canHaveUAVs = false;
		materialBufferDesc.initialState = nvrhi::ResourceStates::ShaderResource;
		materialBufferDesc.keepInitialState = true;
		m_MaterialBuffer = device->createBuffer(materialBufferDesc);
	}

	void RayTracingPipeline::createRayTracingPipeline(nvrhi::DeviceHandle device)
	{
		auto bindingLayoutDesc = nvrhi::BindingLayoutDesc()
			.setVisibility(nvrhi::ShaderType::All)
			.addItem(nvrhi::BindingLayoutItem::Texture_UAV(0))
			.addItem(nvrhi::BindingLayoutItem::RayTracingAccelStruct(0))
			.addItem(nvrhi::BindingLayoutItem::VolatileConstantBuffer(0))
			.addItem(nvrhi::BindingLayoutItem::StructuredBuffer_SRV(1))
			.addItem(nvrhi::BindingLayoutItem::Sampler(0))
			.addItem(nvrhi::BindingLayoutItem::StructuredBuffer_SRV(3));

		m_Layout = device->createBindingLayout(bindingLayoutDesc);

		nvrhi::rt::PipelineDesc pipelineDesc;
		pipelineDesc.globalBindingLayouts = { m_Layout, m_BindlessLayout };

		{
			ShaderInformation gen {};
			gen.Name = "RayTracing.Gen";
			gen.ShaderType = nvrhi::ShaderType::RayGeneration;
			pipelineDesc.addShader(nvrhi::rt::PipelineShaderDesc("", ShaderLibrary::GetShader(gen), nullptr));
		}

		{
			ShaderInformation gen {};
			gen.Name = "RayTracing.Gen";
			gen.ShaderType = nvrhi::ShaderType::Miss;
			pipelineDesc.addShader(nvrhi::rt::PipelineShaderDesc("", ShaderLibrary::GetShader(gen), nullptr));
		}

		nvrhi::ShaderHandle closestHit;
		{
			ShaderInformation gen {};
			gen.Name = "RayTracing.Gen";
			gen.ShaderType = nvrhi::ShaderType::ClosestHit;
			closestHit = ShaderLibrary::GetShader(gen);
		}

		pipelineDesc.addHitGroup({ "CL_Main", closestHit, nullptr, nullptr, nullptr, false });
		pipelineDesc.maxPayloadSize = sizeof(Float4);

		m_Pipeline = device->createRayTracingPipeline(pipelineDesc);

		m_ShaderTable = m_Pipeline->createShaderTable();
		m_ShaderTable->setRayGenerationShader("RG_Main");
		m_ShaderTable->addHitGroup("CL_Main");
		m_ShaderTable->addMissShader("MI_Main");
	}

	RayTracingPipeline::RayTracingPipeline()
	{
		nvrhi::DeviceHandle device = GameEngine::Get().GetDeviceHelper().GetDevice();

		nvrhi::SamplerDesc samplerDesc;
		m_LinearSampler = device->createSampler(samplerDesc);

		m_FrameConstantBuffer = device->createBuffer(nvrhi::utils::CreateVolatileConstantBufferDesc(sizeof(RTFrameData), "RT Frame Data", 16));

		prepareBindlessResources(device);
		createRayTracingPipeline(device);
	}

	void RayTracingPipeline::buildInstances(const ScriptablePipelineRenderingContext& context)
	{
		m_RTInstances.clear();
		m_RTInstances.reserve(context.DrawCalls.size());

		m_InstanceBufferIndices.clear();
		m_InstanceBufferIndices.reserve(context.DrawCalls.size());

		m_RTMaterials.clear();
		m_RTMaterials.reserve(context.DrawCalls.size());

		int32 instanceId = 0;
		for (const DrawCall& call : context.DrawCalls)
		{
			auto instanceDesc = nvrhi::rt::InstanceDesc()
				.setBLAS(call.Mesh->GetBLAS())
				.setFlags(nvrhi::rt::InstanceFlags::TriangleCullDisable)
				.setInstanceMask(1)
				.setInstanceID(instanceId++);

			Matrix4 transform =
				Matrix4::Translate(call.Transform.Position) *
				Matrix4::FromQuaternion(call.Transform.Orientation) *
				Matrix4::Scale(call.Transform.Scale);

			for (int32 row = 0; row < 3; ++row)
			{
				for (int32 col = 0; col < 4; ++col)
				{
					instanceDesc.transform[row * 4 + col] = transform.Values[col][row];
				}
			}

			InstanceBufferIndices instanceIndices = {};
			instanceIndices.vertexBufferIndex =
				m_BindlessManager->CreateDescriptor(
					nvrhi::BindingSetItem::RawBuffer_SRV(0, call.Mesh->VertexBuffer)
				);

			instanceIndices.indexBufferIndex =
				m_BindlessManager->CreateDescriptor(
					nvrhi::BindingSetItem::RawBuffer_SRV(0, call.Mesh->IndexBuffer)
				);

			RTMaterial material;
			material.Color = call.Material.Color.Value;
			if (call.Material.Color.UseMap)
				material.ColorMapIndex = m_BindlessManager->CreateDescriptor(nvrhi::BindingSetItem::Texture_SRV(0, call.Material.Color.Map));

			material.Normal = call.Material.Normal.Value;
			if (call.Material.Normal.UseMap)
				material.NormalMapIndex = m_BindlessManager->CreateDescriptor(nvrhi::BindingSetItem::Texture_SRV(0, call.Material.Normal.Map));

			m_RTInstances.push_back(instanceDesc);
			m_RTMaterials.push_back(material);
			m_InstanceBufferIndices.push_back(instanceIndices);
		}
	}

	void RayTracingPipeline::buildAccelerationStructure(nvrhi::CommandListHandle cmd, nvrhi::DeviceHandle device)
	{
		cmd->writeBuffer(m_InstanceInfoBuffer, m_InstanceBufferIndices.data(), m_InstanceBufferIndices.size() * sizeof(InstanceBufferIndices));
		cmd->writeBuffer(m_MaterialBuffer, m_RTMaterials.data(), m_RTMaterials.size() * sizeof(RTMaterial));

		if (!m_TLAS || m_TLAS->getDesc().topLevelMaxInstances != m_RTInstances.size())
		{
			m_TLAS = nullptr;

			auto tlasDesc = nvrhi::rt::AccelStructDesc()
				.setDebugName("RayTracing TLAS")
				.setIsTopLevel(true)
				.setBuildFlags(nvrhi::rt::AccelStructBuildFlags::AllowUpdate)
				.setTopLevelMaxInstances(m_RTInstances.size());

			m_TLAS = device->createAccelStruct(tlasDesc);

			cmd->buildTopLevelAccelStruct(m_TLAS, m_RTInstances.data(), m_RTInstances.size());
		}
		else
		{
			cmd->buildTopLevelAccelStruct(m_TLAS, m_RTInstances.data(), m_RTInstances.size(), nvrhi::rt::AccelStructBuildFlags::AllowUpdate | nvrhi::rt::AccelStructBuildFlags::PerformUpdate);
		}
	}

	void RayTracingPipeline::dispatchRays(ScriptablePipelineRenderingContext& context, Camera& camera)
	{
		nvrhi::CommandListHandle cmd = context.Context;
		nvrhi::DeviceHandle device = context.Device;

		float yawSign = camera.GetTransform().GetUp().Y < 0 ? -1.0f : 1.0f;
		Float3 lookAt = camera.GetTransform().Position + camera.GetTransform().GetForward();

		Matrix4 projection = Matrix4::Perspective(Math::Radians(90), context.Viewport.X / context.Viewport.Y, 0.01f, 1000.0f);
		Matrix4 cameraProj = Matrix4::LookAt(camera.GetTransform().Position, lookAt, Float3(0, yawSign, 0));

		RTFrameData data {};
		data.View = cameraProj;
		data.Perspective = projection;
		data.ViewInv = Matrix4::Invert(cameraProj);
		data.PerspectiveInv = Matrix4::Invert(projection);
		data.ViewPosition = camera.GetTransform().Position;

		cmd->writeBuffer(m_FrameConstantBuffer, &data, sizeof(RTFrameData));

		if (!m_RenderTarget)
		{
			nvrhi::TextureDesc textureDesc = context.Output->getDesc().colorAttachments[0].texture->getDesc();
			textureDesc.isUAV = true;
			textureDesc.isRenderTarget = false;
			textureDesc.initialState = nvrhi::ResourceStates::UnorderedAccess;
			textureDesc.keepInitialState = true;
			textureDesc.format = nvrhi::Format::RGBA8_UNORM;
			m_RenderTarget = device->createTexture(textureDesc);
		}

		nvrhi::BindingSetDesc setDesc {};
		setDesc.addItem(nvrhi::BindingSetItem::Texture_UAV(0, m_RenderTarget));
		setDesc.addItem(nvrhi::BindingSetItem::RayTracingAccelStruct(0, m_TLAS));
		setDesc.addItem(nvrhi::BindingSetItem::ConstantBuffer(0, m_FrameConstantBuffer));
		setDesc.addItem(nvrhi::BindingSetItem::StructuredBuffer_SRV(1, m_InstanceInfoBuffer));
		setDesc.addItem(nvrhi::BindingSetItem::Sampler(0, m_LinearSampler));
		setDesc.addItem(nvrhi::BindingSetItem::StructuredBuffer_SRV(3, m_MaterialBuffer));

		nvrhi::rt::State state;
		state.setShaderTable(m_ShaderTable);
		state.addBindingSet(DescriptorsCache::Get(device, m_Layout, setDesc));
		state.addBindingSet(m_BindlessManager->GetDescriptorTable());

		cmd->setRayTracingState(state);

		nvrhi::rt::DispatchRaysArguments args;
		args.width = context.Viewport.X;
		args.height = context.Viewport.Y;
		cmd->dispatchRays(args);

		cmd->copyTexture(context.Output->getDesc().colorAttachments[0].texture, nvrhi::TextureSlice {}, m_RenderTarget, nvrhi::TextureSlice {});
	}

	void RayTracingPipeline::Render(ScriptablePipelineRenderingContext& context, Camera& camera)
	{
		nvrhi::CommandListHandle cmd = context.Context;
		nvrhi::DeviceHandle device = context.Device;

		cmd->beginMarker("Path Traced Pipeline");

		buildInstances(context);
		buildAccelerationStructure(cmd, device);
		dispatchRays(context, camera);

		cmd->endMarker();
	}
}
