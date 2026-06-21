#include "ImGui.hpp"

#include <imgui_internal.h>

#include "DescriptorsCache.hpp"
#include "PSOCache.hpp"
#include "ShaderLibrary.hpp"
#include "Core/Math/Float2.hpp"

namespace Hyper
{
	bool ImGuiNVRHI::reallocateBuffer(nvrhi::BufferHandle& buffer, size_t requiredSize, size_t reallocateSize, const bool indexBuffer)
	{
		if (buffer == nullptr || size_t(buffer->getDesc().byteSize) < requiredSize)
		{
			nvrhi::BufferDesc desc;
			desc.byteSize = uint32_t(reallocateSize);
			desc.structStride = 0;
			desc.debugName = indexBuffer ? "ImGui Index Buffer" : "ImGui Vertex Buffer";
			desc.canHaveUAVs = false;
			desc.isVertexBuffer = !indexBuffer;
			desc.isIndexBuffer = indexBuffer;
			desc.isDrawIndirectArgs = false;
			desc.isVolatile = false;
			desc.initialState = indexBuffer ? nvrhi::ResourceStates::IndexBuffer : nvrhi::ResourceStates::VertexBuffer;
			desc.keepInitialState = true;

			buffer = m_device->createBuffer(desc);

			if (!buffer)
			{
				return false;
			}
		}

		return true;
	}
	void ImGuiNVRHI::Init(nvrhi::IDevice* device)
	{
		m_device = device;

		nvrhi::VertexAttributeDesc vertexAttribLayout[] = {
	{ "POSITION", nvrhi::Format::RG32_FLOAT,  1, 0, offsetof(ImDrawVert,pos), sizeof(ImDrawVert), false },
	{ "TEXCOORD", nvrhi::Format::RG32_FLOAT,  1, 0, offsetof(ImDrawVert,uv),  sizeof(ImDrawVert), false },
	{ "COLOR",    nvrhi::Format::RGBA8_UNORM, 1, 0, offsetof(ImDrawVert,col), sizeof(ImDrawVert), false },
		};

		shaderAttribLayout = m_device->createInputLayout(vertexAttribLayout, sizeof(vertexAttribLayout) / sizeof(vertexAttribLayout[0]), vertexShader);

		{
			ShaderInformation information { };
			information.Name = "ImGui";
			information.ShaderType = nvrhi::ShaderType::Vertex;
			vertexShader = ShaderLibrary::GetShader(information);
		}

		{
			ShaderInformation information { };
			information.Name = "ImGui";
			information.ShaderType = nvrhi::ShaderType::Pixel;
			pixelShader = ShaderLibrary::GetShader(information);
		}

		{
			nvrhi::BlendState blendState;
			blendState.targets[0].setBlendEnable(true)
				.setSrcBlend(nvrhi::BlendFactor::SrcAlpha)
				.setDestBlend(nvrhi::BlendFactor::InvSrcAlpha)
				.setSrcBlendAlpha(nvrhi::BlendFactor::InvSrcAlpha)
				.setDestBlendAlpha(nvrhi::BlendFactor::Zero);

			auto rasterState = nvrhi::RasterState()
				.setFillSolid()
				.setCullNone()
				.setScissorEnable(true)
				.setDepthClipEnable(true);

			auto depthStencilState = nvrhi::DepthStencilState()
				.disableDepthTest()
				.enableDepthWrite()
				.disableStencil()
				.setDepthFunc(nvrhi::ComparisonFunc::Always);

			nvrhi::RenderState renderState;
			renderState.blendState = blendState;
			renderState.depthStencilState = depthStencilState;
			renderState.rasterState = rasterState;

			nvrhi::BindingLayoutDesc layoutDesc;
			layoutDesc.visibility = nvrhi::ShaderType::All;
			layoutDesc.bindings = {
				nvrhi::BindingLayoutItem::PushConstants(0, sizeof(Float2)),
				nvrhi::BindingLayoutItem::Texture_SRV(0),
				nvrhi::BindingLayoutItem::Sampler(0)
			};
			bindingLayout = m_device->createBindingLayout(layoutDesc);

			basePSODesc.primType = nvrhi::PrimitiveType::TriangleList;
			basePSODesc.inputLayout = shaderAttribLayout;
			basePSODesc.VS = vertexShader;
			basePSODesc.PS = pixelShader;
			basePSODesc.renderState = renderState;
			basePSODesc.bindingLayouts = { bindingLayout };
		}

		{
			const auto desc = nvrhi::SamplerDesc()
				.setAllAddressModes(nvrhi::SamplerAddressMode::Wrap)
				.setAllFilters(true);

			fontSampler = m_device->createSampler(desc);
		}
	}

	void ImGuiNVRHI::updateGeometry(ImGuiContext* imContext, MinimalGraphicsContext& context)
	{
		nvrhi::CommandListHandle cmd = context.Context;
		ImDrawData* drawData = ImGui::GetDrawData();

		reallocateBuffer(vertexBuffer,
			drawData->TotalVtxCount * sizeof(ImDrawVert),
			(drawData->TotalVtxCount + 5000) * sizeof(ImDrawVert),
			false);

		reallocateBuffer(indexBuffer,
			drawData->TotalIdxCount * sizeof(ImDrawIdx),
			(drawData->TotalIdxCount + 5000) * sizeof(ImDrawIdx),
			true);

		vtxBuffer.resize(vertexBuffer->getDesc().byteSize / sizeof(ImDrawVert));
		idxBuffer.resize(indexBuffer->getDesc().byteSize / sizeof(ImDrawIdx));

		ImDrawVert* vtxDst = &vtxBuffer[0];
		ImDrawIdx* idxDst = &idxBuffer[0];

		for (int n = 0; n < drawData->CmdListsCount; n++)
		{
			const ImDrawList* cmdList = drawData->CmdLists[n];

			memcpy(vtxDst, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
			memcpy(idxDst, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));

			vtxDst += cmdList->VtxBuffer.Size;
			idxDst += cmdList->IdxBuffer.Size;
		}

		cmd->writeBuffer(vertexBuffer, &vtxBuffer[0], vertexBuffer->getDesc().byteSize);
		cmd->writeBuffer(indexBuffer, &idxBuffer[0], indexBuffer->getDesc().byteSize);
	}


	void ImGuiNVRHI::UpdateFontTexture(ImGuiContext* imContext, MinimalGraphicsContext& context)
	{
		nvrhi::CommandListHandle cmd = context.Context;
		ImGuiIO& io = imContext->IO;

		if (fontTexture && io.Fonts->TexID)
			return;

		io.Fonts->AddFontDefault();
		io.Fonts->AddFontFromFileTTF("font.ttf", 16);

		unsigned char* pixels;
		int width, height;

		int stride;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &stride);

		nvrhi::TextureDesc textureDesc;
		textureDesc.width = width;
		textureDesc.height = height;
		textureDesc.format = nvrhi::Format::RGBA8_UNORM;
		textureDesc.initialState = nvrhi::ResourceStates::ShaderResource;
		textureDesc.keepInitialState = true;
		textureDesc.debugName = "ImGui Font Atlas";

		fontTexture = m_device->createTexture(textureDesc);

		cmd->writeTexture(fontTexture, 0, 0, pixels, width * stride);


		io.Fonts->SetTexID(fontTexture.Get());
	}

	void ImGuiNVRHI::Render(ImGuiContext* imContext, MinimalGraphicsContext& context)
	{
		nvrhi::CommandListHandle cmd = context.Context;
		ImGui::SetCurrentContext(imContext);

		ImDrawData* drawData = ImGui::GetDrawData();
		const auto& io = imContext->IO;


		updateGeometry(imContext, context);

		drawData->ScaleClipRects(io.DisplayFramebufferScale);

		float invDisplaySize[2] = { 1.f / io.DisplaySize.x, 1.f / io.DisplaySize.y };

		nvrhi::GraphicsState drawState;

		drawState.framebuffer = context.State.framebuffer;
		drawState.pipeline = PSOCache::Get(context.Device, context.State.framebuffer, basePSODesc);

		drawState.viewport.viewports.push_back(nvrhi::Viewport(io.DisplaySize.x * io.DisplayFramebufferScale.x,
			io.DisplaySize.y * io.DisplayFramebufferScale.y));
		drawState.viewport.scissorRects.resize(1);  // updated below

		nvrhi::VertexBufferBinding vbufBinding;
		vbufBinding.buffer = vertexBuffer;
		vbufBinding.slot = 0;
		vbufBinding.offset = 0;
		drawState.vertexBuffers.push_back(vbufBinding);

		drawState.indexBuffer.buffer = indexBuffer;
		drawState.indexBuffer.format = (sizeof(ImDrawIdx) == 2 ? nvrhi::Format::R16_UINT : nvrhi::Format::R32_UINT);
		drawState.indexBuffer.offset = 0;

		int vtxOffset = 0;
		int idxOffset = 0;
		for (int n = 0; n < drawData->CmdListsCount; n++)
		{
			const ImDrawList* cmdList = drawData->CmdLists[n];
			for (int i = 0; i < cmdList->CmdBuffer.Size; i++)
			{
				const ImDrawCmd* pCmd = &cmdList->CmdBuffer[i];

				if (pCmd->UserCallback)
				{
					pCmd->UserCallback(cmdList, pCmd);
				}
				else
				{
					drawState.bindings = {
						DescriptorsCache::Get(context.Device, bindingLayout,nvrhi::BindingSetDesc().
							addItem(nvrhi::BindingSetItem::PushConstants(0, sizeof(Float2))).
							addItem(nvrhi::BindingSetItem::Texture_SRV(0, ( nvrhi::ITexture* )pCmd->TextureId)).
							addItem(nvrhi::BindingSetItem::Sampler(0, fontSampler)))
					};

					drawState.viewport.scissorRects[0] = nvrhi::Rect(int(pCmd->ClipRect.x),
						int(pCmd->ClipRect.z),
						int(pCmd->ClipRect.y),
						int(pCmd->ClipRect.w));

					nvrhi::DrawArguments drawArguments;
					drawArguments.vertexCount = pCmd->ElemCount;
					drawArguments.startIndexLocation = idxOffset;
					drawArguments.startVertexLocation = vtxOffset;

					cmd->setGraphicsState(drawState);
					cmd->setPushConstants(invDisplaySize, sizeof(invDisplaySize));
					cmd->drawIndexed(drawArguments);
				}

				idxOffset += pCmd->ElemCount;
			}

			vtxOffset += cmdList->VtxBuffer.Size;
		}
	}
}
