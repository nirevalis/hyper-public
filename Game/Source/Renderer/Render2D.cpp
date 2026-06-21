#include "Render2D.hpp"
#include "ShaderLibrary.hpp"
#include "PSOCache.hpp"
#include "DescriptorsCache.hpp"

namespace Hyper
{
	bool CanBatch(Render2DDrawCall& call, Render2DDrawCall& secondCall)
	{
		if (call.Type != secondCall.Type)
			return false;

		switch (call.Type)
		{
		case DrawCallType::RectangleColored:
			break;
		case DrawCallType::RectangleRounded:
			break;
		case DrawCallType::TextCharacter:
			if (call.Texture != secondCall.Texture)
				return false;
			break;
		case DrawCallType::RectangleTextured:
			if (call.Texture != secondCall.Texture)
				return false;
			break;
		}

		return true;
	}

	void Render2D::DrawBatch(MinimalGraphicsContext& context, int32 batchStart, int32 batchSize)
	{
		Matrix4 projection = Matrix4::Identity;

		for (const Matrix4& transform : m_TransformStack)
		{
			projection = projection * transform;
		}

		projection = projection * m_Projection;

		int32 countIb = 0;
		for (int32 i = 0; i < batchSize; i++)
		{
			countIb += m_DrawCalls[batchStart + i].IndexCount;
		}

		if (!countIb)
		{
			return;
		}

		auto cmd = context.Context;
		auto graphicsState = context.State;

		switch (m_DrawCalls[batchStart].Type)
		{
		case DrawCallType::RectangleColored:
		{
			graphicsState.setPipeline(PSOCache::Get(context.Device, context.State.framebuffer, m_QuadPSO)).addBindingSet(m_QuadPSOSet);
			break;
		}
		case DrawCallType::RectangleTextured:
		{
			auto desc = nvrhi::BindingSetDesc()
				.addItem(nvrhi::BindingSetItem::PushConstants(0, sizeof(Matrix4)))
				.addItem(nvrhi::BindingSetItem::Texture_SRV(1, m_DrawCalls[batchStart].Texture))
				.addItem(nvrhi::BindingSetItem::Sampler(2, m_LinearSampler));

			graphicsState.setPipeline(PSOCache::Get(context.Device, context.State.framebuffer, m_QuadTexturePSO)).addBindingSet(DescriptorsCache::Get(context.Device, m_QuadTexturePSOLayout, desc));
			break;
		}
		case DrawCallType::TextCharacter:
		{
			auto desc = nvrhi::BindingSetDesc()
			.addItem(nvrhi::BindingSetItem::PushConstants(0, sizeof(Matrix4)))
			.addItem(nvrhi::BindingSetItem::Texture_SRV(1, m_DrawCalls[batchStart].Texture))
			.addItem(nvrhi::BindingSetItem::Sampler(2, m_LinearSampler));

			graphicsState.setPipeline(PSOCache::Get(context.Device, context.State.framebuffer, m_QuadFontPSO)).addBindingSet(DescriptorsCache::Get(context.Device, m_QuadFontPSOLayout, desc));
			break;
		}
		case DrawCallType::RectangleRounded:
		{
			auto desc = nvrhi::BindingSetDesc()
			.addItem(nvrhi::BindingSetItem::PushConstants(0, sizeof(Matrix4)))
			.addItem(nvrhi::BindingSetItem::StructuredBuffer_SRV(0, m_MetadataBuffer));

			graphicsState.setPipeline(PSOCache::Get(context.Device, context.State.framebuffer, m_QuadRoundedPSO)).addBindingSet(DescriptorsCache::Get(context.Device, m_QuadRoundedPSOLayout, desc));
			break;
		}
		}

		graphicsState.addVertexBuffer(nvrhi::VertexBufferBinding(m_VB));
		graphicsState.setIndexBuffer(nvrhi::IndexBufferBinding(m_IB).setFormat(nvrhi::Format::R32_UINT));
		cmd->setGraphicsState(graphicsState);

		cmd->setPushConstants(&projection, sizeof(Matrix4));

		auto drawArguments = nvrhi::DrawArguments();
		drawArguments.setVertexCount(countIb);
		drawArguments.setStartIndexLocation(m_DrawCalls[batchStart].IndexOffset);
		cmd->drawIndexed(drawArguments);
	}

	Render2D::Render2D(nvrhi::DeviceHandle device)
	{
		auto applyBlending = [](nvrhi::GraphicsPipelineDesc& desc)
		{
			nvrhi::BlendState::RenderTarget& rt = desc.renderState.blendState.targets[0];
			rt.blendEnable = true;
			rt.srcBlend = nvrhi::BlendFactor::SrcAlpha;
			rt.destBlend = nvrhi::BlendFactor::InvSrcAlpha;
			rt.srcBlendAlpha = nvrhi::BlendFactor::One;
			rt.destBlendAlpha = nvrhi::BlendFactor::Zero;
			rt.blendOpAlpha = nvrhi::BlendOp::Add;
			rt.colorWriteMask = nvrhi::ColorMask::All;
		};

		nvrhi::SamplerDesc samplerDesc;
		m_LinearSampler = device->createSampler(samplerDesc);

		nvrhi::VertexAttributeDesc attributes[] = {
			nvrhi::VertexAttributeDesc()
				.setName("POSITION")
				.setFormat(nvrhi::Format::RG32_FLOAT)
				.setOffset(offsetof(Vertex2D, Position))
				.setElementStride(sizeof(Vertex2D)),
			nvrhi::VertexAttributeDesc()
				.setName("TEXCOORD")
				.setFormat(nvrhi::Format::RG32_FLOAT)
				.setOffset(offsetof(Vertex2D, TexCoord))
				.setElementStride(sizeof(Vertex2D)),
			nvrhi::VertexAttributeDesc()
				.setName("COLOR")
				.setFormat(nvrhi::Format::RGBA32_FLOAT)
				.setOffset(offsetof(Vertex2D, Tint))
				.setElementStride(sizeof(Vertex2D)),
			nvrhi::VertexAttributeDesc()
				.setName("MY_SEMANTIC")
				.setFormat(nvrhi::Format::R32_UINT)
				.setOffset(offsetof(Vertex2D, Metadata))
				.setElementStride(sizeof(Vertex2D)),
		};

		m_VertexLayout = device->createInputLayout(attributes, 4u, nullptr);

		{
			auto desc = nvrhi::GraphicsPipelineDesc();
			desc.setInputLayout(m_VertexLayout)
				.renderState.rasterState.setCullNone();

			desc.renderState.depthStencilState.depthTestEnable = false;
			desc.renderState.depthStencilState.depthWriteEnable = false;
			desc.renderState.depthStencilState.stencilEnable = false;

			applyBlending(desc);

			{
				ShaderInformation quadShaderVS {  };
				quadShaderVS.Name = "Render2D.Quad";
				quadShaderVS.ShaderType = nvrhi::ShaderType::Vertex;
				desc.setVertexShader(ShaderLibrary::GetShader(quadShaderVS));
			}

			{
				ShaderInformation quadShaderPS {  };
				quadShaderPS.Name = "Render2D.Quad";
				quadShaderPS.ShaderType = nvrhi::ShaderType::Pixel;
				desc.setPixelShader(ShaderLibrary::GetShader(quadShaderPS));
			}

			auto layoutDesc = nvrhi::BindingLayoutDesc()
				.setVisibility(nvrhi::ShaderType::All)
				.addItem(nvrhi::BindingLayoutItem::PushConstants(0, sizeof(Matrix4)));

			m_QuadPSOLayout = device->createBindingLayout(layoutDesc);
			desc.addBindingLayout(m_QuadPSOLayout);

			auto bindingSetDesc = nvrhi::BindingSetDesc()
				.addItem(nvrhi::BindingSetItem::PushConstants(0, sizeof(Matrix4)));

			m_QuadPSOSet = device->createBindingSet(bindingSetDesc, m_QuadPSOLayout);
			m_QuadPSO = desc;
		}

		{
			auto desc = nvrhi::GraphicsPipelineDesc();
			desc.setInputLayout(m_VertexLayout)
				.renderState.rasterState.setCullNone();

			desc.renderState.depthStencilState.depthTestEnable = false;
			desc.renderState.depthStencilState.depthWriteEnable = false;
			desc.renderState.depthStencilState.stencilEnable = false;

			applyBlending(desc);

			{
				ShaderInformation quadShaderVS {  };
				quadShaderVS.Name = "Render2D.QuadRounded";
				quadShaderVS.ShaderType = nvrhi::ShaderType::Vertex;
				desc.setVertexShader(ShaderLibrary::GetShader(quadShaderVS));
			}

			{
				ShaderInformation quadShaderPS {  };
				quadShaderPS.Name = "Render2D.QuadRounded";
				quadShaderPS.ShaderType = nvrhi::ShaderType::Pixel;
				desc.setPixelShader(ShaderLibrary::GetShader(quadShaderPS));
			}

			auto layoutDesc = nvrhi::BindingLayoutDesc()
				.setVisibility(nvrhi::ShaderType::All)
				.addItem(nvrhi::BindingLayoutItem::PushConstants(0, sizeof(Matrix4)))
				.addItem(nvrhi::BindingLayoutItem::StructuredBuffer_SRV(0));

			m_QuadRoundedPSOLayout = device->createBindingLayout(layoutDesc);
			desc.addBindingLayout(m_QuadRoundedPSOLayout);

			auto bindingSetDesc = nvrhi::BindingSetDesc()
				.addItem(nvrhi::BindingSetItem::PushConstants(0, sizeof(Matrix4)));

			m_QuadRoundedPSO = desc;
		}

		{
			auto desc = nvrhi::GraphicsPipelineDesc();
			desc.setInputLayout(m_VertexLayout)
				.renderState.rasterState.setCullNone();

			desc.renderState.depthStencilState.depthTestEnable = false;
			desc.renderState.depthStencilState.depthWriteEnable = false;
			desc.renderState.depthStencilState.stencilEnable = false;

			applyBlending(desc);

			{
				ShaderInformation quadShaderVS {  };
				quadShaderVS.Name = "Render2D.QuadTexture";
				quadShaderVS.ShaderType = nvrhi::ShaderType::Vertex;
				desc.setVertexShader(ShaderLibrary::GetShader(quadShaderVS));
			}

			{
				ShaderInformation quadShaderPS {  };
				quadShaderPS.Name = "Render2D.QuadTexture";
				quadShaderPS.ShaderType = nvrhi::ShaderType::Pixel;
				desc.setPixelShader(ShaderLibrary::GetShader(quadShaderPS));
			}

			auto layoutDesc = nvrhi::BindingLayoutDesc()
				.setVisibility(nvrhi::ShaderType::All)
				.addItem(nvrhi::BindingLayoutItem::PushConstants(0, sizeof(Matrix4)))
				.addItem(nvrhi::BindingLayoutItem::Texture_SRV(1))
				.addItem(nvrhi::BindingLayoutItem::Sampler(2));

			m_QuadTexturePSOLayout = device->createBindingLayout(layoutDesc);
			desc.addBindingLayout(m_QuadTexturePSOLayout);

			m_QuadTexturePSO = desc;
		}

		{
			auto desc = nvrhi::GraphicsPipelineDesc();
			desc.setInputLayout(m_VertexLayout)
				.renderState.rasterState.setCullNone();

			desc.renderState.depthStencilState.depthTestEnable = false;
			desc.renderState.depthStencilState.depthWriteEnable = false;
			desc.renderState.depthStencilState.stencilEnable = false;

			applyBlending(desc);

			{
				ShaderInformation quadShaderVS {  };
				quadShaderVS.Name = "Render2D.QuadFont";
				quadShaderVS.ShaderType = nvrhi::ShaderType::Vertex;
				desc.setVertexShader(ShaderLibrary::GetShader(quadShaderVS));
			}

			{
				ShaderInformation quadShaderPS {  };
				quadShaderPS.Name = "Render2D.QuadFont";
				quadShaderPS.ShaderType = nvrhi::ShaderType::Pixel;
				desc.setPixelShader(ShaderLibrary::GetShader(quadShaderPS));
			}

			auto layoutDesc = nvrhi::BindingLayoutDesc()
				.setVisibility(nvrhi::ShaderType::All)
				.addItem(nvrhi::BindingLayoutItem::PushConstants(0, sizeof(Matrix4)))
				.addItem(nvrhi::BindingLayoutItem::Texture_SRV(1))
				.addItem(nvrhi::BindingLayoutItem::Sampler(2));

			m_QuadFontPSOLayout = device->createBindingLayout(layoutDesc);
			desc.addBindingLayout(m_QuadFontPSOLayout);

			m_QuadFontPSO = desc;
		}
	}

	void Render2D::Begin(MinimalGraphicsContext& context)
	{
		m_Projection = Matrix4::Orthographic(
			0,
			context.State.viewport.viewports.at(0).width(),
			context.State.viewport.viewports.at(0).height(),
			0, 1, -1
			);
	}

	void Render2D::DrawRoundedRect(const Rectangle &rect, const Color &c, float rounding)
	{
		Render2DDrawCall call {};
		call.IndexOffset = m_IndexOffset;
		call.IndexCount = 6;
		call.Type = DrawCallType::RectangleRounded;

		Render2DObjectMetadata metadata ={};
		metadata.RoundedRect.Width = rect.Width;
		metadata.RoundedRect.Height = rect.Height;
		metadata.RoundedRect.Radius = rounding;

		m_Vertices.emplace_back(rect.X + rect.Width, rect.Y + rect.Height, 1, 1, c, m_Metadatas.size());
		m_Vertices.emplace_back(rect.X, rect.Y + rect.Height, 0, 1, c, m_Metadatas.size());
		m_Vertices.emplace_back(rect.X, rect.Y, 0, 0, c, m_Metadatas.size());
		m_Vertices.emplace_back(rect.X + rect.Width, rect.Y, 1, 0, c, m_Metadatas.size());

		m_Metadatas.push_back(metadata);

		m_Indices.push_back(m_VertexOffset + 0);
		m_Indices.push_back(m_VertexOffset + 1);
		m_Indices.push_back(m_VertexOffset + 2);
		m_Indices.push_back(m_VertexOffset + 2);
		m_Indices.push_back(m_VertexOffset + 3);
		m_Indices.push_back(m_VertexOffset + 0);

		m_IndexOffset += 6;
		m_VertexOffset += 4;

		m_DrawCalls.emplace_back(call);
	}


	void Render2D::DrawRect(const Rectangle& rect, const Color& c)
	{
		Render2DDrawCall call {};
		call.IndexOffset = m_IndexOffset;
		call.IndexCount = 6;
		call.Type = DrawCallType::RectangleColored;

		m_Vertices.push_back(Vertex2D(rect.X + rect.Width, rect.Y + rect.Height, 1, 1, c));
		m_Vertices.push_back(Vertex2D(rect.X, rect.Y + rect.Height, 0, 1, c));
		m_Vertices.push_back(Vertex2D(rect.X, rect.Y, 0, 0, c));
		m_Vertices.push_back(Vertex2D(rect.X + rect.Width, rect.Y, 1, 0, c));

		m_Indices.push_back(m_VertexOffset + 0);
		m_Indices.push_back(m_VertexOffset + 1);
		m_Indices.push_back(m_VertexOffset + 2);
		m_Indices.push_back(m_VertexOffset + 2);
		m_Indices.push_back(m_VertexOffset + 3);
		m_Indices.push_back(m_VertexOffset + 0);

		m_IndexOffset += 6;
		m_VertexOffset += 4;

		m_DrawCalls.emplace_back(call);
	}

	void Render2D::DrawTexture(const Rectangle& rect, nvrhi::TextureHandle texture)
	{
		Render2DDrawCall call {};
		call.IndexOffset = m_IndexOffset;
		call.IndexCount = 6;
		call.Type = DrawCallType::RectangleTextured;
		call.Texture = texture;

		m_Vertices.push_back(Vertex2D(rect.X + rect.Width, rect.Y + rect.Height, 1, 1));
		m_Vertices.push_back(Vertex2D(rect.X, rect.Y + rect.Height, 0, 1));
		m_Vertices.push_back(Vertex2D(rect.X, rect.Y, 0, 0));
		m_Vertices.push_back(Vertex2D(rect.X + rect.Width, rect.Y, 1, 0));

		m_Indices.push_back(m_VertexOffset + 0);
		m_Indices.push_back(m_VertexOffset + 1);
		m_Indices.push_back(m_VertexOffset + 2);
		m_Indices.push_back(m_VertexOffset + 2);
		m_Indices.push_back(m_VertexOffset + 3);
		m_Indices.push_back(m_VertexOffset + 0);

		m_IndexOffset += 6;
		m_VertexOffset += 4;

		m_DrawCalls.emplace_back(call);
	}

	void Render2D::DrawText(const String& text, Float2 position, const Color& color, Font& font, float scale, bool centeredX, bool centeredY)
	{
		const auto& fontGeometry = font.Geometry;
		const auto& metrics = fontGeometry.getMetrics();
		nvrhi::TextureHandle atlas = font.Atlas;

		Matrix4 transform = Matrix4::Scale(Float3(scale, -scale,1));

		double x = 0;
		double fsScale = (1.0 / (metrics.ascenderY - metrics.descenderY));
		double y = -metrics.lineHeight;

		position = position - Float2(
			centeredX ? font.GetWidth(text, scale) * 0.5f : 0,
			scale * 0.5f
			);

		const float spaceGlyphAdvance = fontGeometry.getGlyph(' ')->getAdvance();

		for (size_t i = 0; i < text.GetLength(); i++)
		{
			char character = text.Get()[i];
			if (character == '\r')
				continue;

			if (character == '\n')
			{
				x = 0;
				y -= fsScale * metrics.lineHeight;
				continue;
			}

			if (character == ' ')
			{
				float advance = spaceGlyphAdvance;
				if (i < text.GetLength() - 1)
				{
					char nextCharacter = text.Get()[i + 1];
					double dAdvance;
					fontGeometry.getAdvance(dAdvance, character, nextCharacter);
					advance = (float)dAdvance;
				}

				x += fsScale * advance;
				continue;
			}

			if (character == '\t')
			{
				x += 4.0f * (fsScale * spaceGlyphAdvance);
				continue;
			}

			auto glyph = fontGeometry.getGlyph(character);
			if (!glyph)
				glyph = fontGeometry.getGlyph('?');
			if (!glyph)
				return;

			double al, ab, ar, at;
			glyph->getQuadAtlasBounds(al, ab, ar, at);
			Float2 texCoordMin((float)al, (float)ab);
			Float2 texCoordMax((float)ar, (float)at);

			double pl, pb, pr, pt;
			glyph->getQuadPlaneBounds(pl, pb, pr, pt);
			Float2 quadMin((float)pl, (float)pb);
			Float2 quadMax((float)pr, (float)pt);

			quadMin = quadMin * fsScale, quadMax = quadMax * fsScale;
			quadMin = quadMin + Float2(x, y);
			quadMax = quadMax + Float2(x, y);

			float texelWidth = 1.0f / atlas->getDesc().width;
			float texelHeight = 1.0f / atlas->getDesc().height;
			texCoordMin = texCoordMin * Float2(texelWidth, texelHeight);
			texCoordMax = texCoordMax * Float2(texelWidth, texelHeight);

			Render2DDrawCall call {};
			call.IndexOffset = m_IndexOffset;
			call.IndexCount = 6;
			call.Type = DrawCallType::TextCharacter;
			call.Texture = atlas;

			m_Vertices.emplace_back(transform,position,quadMax.X, quadMax.Y, texCoordMax.X, texCoordMax.Y, color); //good
			m_Vertices.emplace_back(transform,position,quadMin.X, quadMax.Y, texCoordMin.X, texCoordMax.Y, color); //good
			m_Vertices.emplace_back(transform,position,quadMin.X, quadMin.Y, texCoordMin.X, texCoordMin.Y, color); //good
			m_Vertices.emplace_back(transform,position,quadMax.X, quadMin.Y, texCoordMax.X, texCoordMin.Y, color); //good

			m_Indices.push_back(m_VertexOffset + 0);
			m_Indices.push_back(m_VertexOffset + 1);
			m_Indices.push_back(m_VertexOffset + 2);
			m_Indices.push_back(m_VertexOffset + 2);
			m_Indices.push_back(m_VertexOffset + 3);
			m_Indices.push_back(m_VertexOffset + 0);

			m_IndexOffset += 6;
			m_VertexOffset += 4;

			m_DrawCalls.emplace_back(call);

			if (i < text.GetLength() - 1)
			{
				double advance = glyph->getAdvance();
				char nextCharacter = text.Get()[i + 1];
				fontGeometry.getAdvance(advance, character, nextCharacter);

				x += fsScale * advance;
			}
		}
	}

	void Render2D::Execute(MinimalGraphicsContext& context)
	{
		context.Context->beginMarker("Render2D");

		if (!m_VB || m_VB->getDesc().byteSize < m_Vertices.size() * sizeof(Vertex2D))
		{
			auto vertexBufferDesc = nvrhi::BufferDesc()
				.setByteSize(m_Vertices.size() * sizeof(Vertex2D))
				.setIsVertexBuffer(true)
				.setInitialState(nvrhi::ResourceStates::VertexBuffer)
				.setKeepInitialState(true)
				.setDebugName("Render2D Vertex Buffer");

			m_VB = context.Device->createBuffer(vertexBufferDesc);
		}
		context.Context->writeBuffer(m_VB, m_Vertices.data(), m_Vertices.size() * sizeof(Vertex2D));

		if (!m_IB || m_IB->getDesc().byteSize < m_Indices.size() * sizeof(uint32))
		{
			auto indexBufferDesc = nvrhi::BufferDesc()
				.setByteSize(m_Indices.size() * sizeof(uint32))
				.setIsIndexBuffer(true)
				.setInitialState(nvrhi::ResourceStates::IndexBuffer)
				.setKeepInitialState(true)
				.setDebugName("Render2D Index Buffer");

			m_IB = context.Device->createBuffer(indexBufferDesc);
		}

		context.Context->writeBuffer(m_IB, m_Indices.data(), m_Indices.size() * sizeof(uint32));

		if (!m_MetadataBuffer || m_MetadataBuffer->getDesc().byteSize < m_Metadatas.size() * sizeof(Render2DObjectMetadata))
		{
			auto metaBufferDesc = nvrhi::BufferDesc()
				.setStructStride(sizeof(Render2DObjectMetadata))
				.setByteSize(m_Metadatas.size() * sizeof(Render2DObjectMetadata))
				.setInitialState(nvrhi::ResourceStates::ShaderResource)
				.setKeepInitialState(true)
				.setDebugName("Render2D Meta Buffer");

			m_MetadataBuffer = context.Device->createBuffer(metaBufferDesc);
		}

		context.Context->writeBuffer(m_MetadataBuffer, m_Metadatas.data(), m_Metadatas.size() * sizeof(Render2DObjectMetadata));

		int32 batchStart = 0;
		int32 batchSize = 0;
		for (int32 i = 0; i < m_DrawCalls.size(); i++)
		{
			Render2DDrawCall& call = m_DrawCalls[i];

			if (batchSize != 0 && !CanBatch(call, m_DrawCalls[batchStart]))
			{
				DrawBatch(context, batchStart, batchSize);
				batchStart += batchSize;
				batchSize = 0;
			}

			batchSize++;
		}

		if (batchSize != 0)
		{
			DrawBatch(context, batchStart, batchSize);
		}

		m_DrawCalls.clear();
		m_Vertices.clear();
		m_Indices.clear();
		m_Metadatas.clear();
		m_TransformStack.clear();
		m_IndexOffset = 0;
		m_VertexOffset = 0;

		context.Context->endMarker();
	}
}