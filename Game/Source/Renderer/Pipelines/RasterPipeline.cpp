#include "RasterPipeline.hpp"

#include "Core/Math/Matrix4.hpp"
#include "Engine/GameEngine.hpp"
#include "Renderer/DescriptorsCache.hpp"
#include "Renderer/ShaderLibrary.hpp"
#include <Renderer/Mesh.hpp>

#include "Asset/AssetRegistry.hpp"
#include "nvrhi/utils.h"
#include "Renderer/PSOCache.hpp"

namespace Hyper
{
	nvrhi::GraphicsPipelineDesc desc;
	nvrhi::InputLayoutHandle m_VertexLayout;

	nvrhi::BindingLayoutHandle layout;
	nvrhi::BindingSetHandle set;

	RasterPipeline::RasterPipeline()
	{
		nvrhi::DeviceHandle device = GameEngine::Get().GetDeviceHelper().GetDevice();

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

		m_VertexLayout = device->createInputLayout(attributes, 3u, nullptr);

		{
			desc = nvrhi::GraphicsPipelineDesc();
			desc.setInputLayout(m_VertexLayout)
				.renderState.rasterState.setCullNone();

			desc.renderState.depthStencilState.depthTestEnable = false;
			desc.renderState.depthStencilState.depthWriteEnable = false;
			desc.renderState.depthStencilState.stencilEnable = false;

			{
				ShaderInformation quadShaderVS {  };
				quadShaderVS.Name = "RasterTesting";
				quadShaderVS.ShaderType = nvrhi::ShaderType::Vertex;
				desc.setVertexShader(ShaderLibrary::GetShader(quadShaderVS));
			}

			{
				ShaderInformation quadShaderPS {  };
				quadShaderPS.Name = "RasterTesting";
				quadShaderPS.ShaderType = nvrhi::ShaderType::Pixel;
				desc.setPixelShader(ShaderLibrary::GetShader(quadShaderPS));
			}

			auto layoutDesc = nvrhi::BindingLayoutDesc()
				.setVisibility(nvrhi::ShaderType::All)
				.addItem(nvrhi::BindingLayoutItem::PushConstants(0, sizeof(Matrix4)));

			layout = device->createBindingLayout(layoutDesc);
			desc.addBindingLayout(layout);

			auto bindingSetDesc = nvrhi::BindingSetDesc()
				.addItem(nvrhi::BindingSetItem::PushConstants(0, sizeof(Matrix4)));

			set = device->createBindingSet(bindingSetDesc, layout);
		}
	}

	void RasterPipeline::Render(ScriptablePipelineRenderingContext& context, Camera& camera)
	{
		nvrhi::DeviceHandle device = context.Device;
		nvrhi::CommandListHandle cmd = context.Context;

		float yawSign = camera.GetTransform().GetUp().Y < 0 ? -1.0f : 1.0f;
		Float3 lookAt = camera.GetTransform().Position + camera.GetTransform().GetForward();

		Matrix4 projection = Matrix4::Perspective(Math::Radians(90), context.Viewport.X / context.Viewport.Y, 0.01f, 1000.0f);
		Matrix4 cameraProj = Matrix4::LookAt(camera.GetTransform().Position, lookAt, Float3(0, yawSign, 0));

		nvrhi::GraphicsState state;
		state.setViewport(nvrhi::ViewportState().addViewportAndScissorRect(nvrhi::Viewport { context.Viewport.X, context.Viewport.Y }));
		state.setFramebuffer(context.Output);
		state.setPipeline(PSOCache::Get(device, context.Output, desc));

		for (const DrawCall& call : context.DrawCalls)
		{
			nvrhi::GraphicsState callState = state;
			callState.addVertexBuffer(nvrhi::VertexBufferBinding(call.Mesh->VertexBuffer));
			callState.setIndexBuffer(nvrhi::IndexBufferBinding(call.Mesh->IndexBuffer).setFormat(nvrhi::Format::R32_UINT));
			callState.addBindingSet(set);

			cmd->setGraphicsState(callState);

			Matrix4 transform = Matrix4::Identity;
			transform = transform * Matrix4::Translate(call.Transform.Position);
			transform = transform * Matrix4::FromQuaternion(call.Transform.Orientation);
			transform = transform * Matrix4::Scale(call.Transform.Scale);

			transform = transform * cameraProj * projection;

			cmd->setPushConstants(&transform, sizeof(Matrix4));

			auto drawArguments = nvrhi::DrawArguments();
			drawArguments.setVertexCount(call.Mesh->IndexBuffer.Get()->getDesc().byteSize / sizeof(uint32));
			cmd->drawIndexed(drawArguments);
		}
	}
}
