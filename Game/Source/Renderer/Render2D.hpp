#pragma once

#include <Core/Common.hpp>
#include <Core/Math/Rectangle.hpp>
#include <Core/Math/Float2.hpp>
#include <Core/Math/Color.hpp>
#include <nvrhi/nvrhi.h>
#include <Core/Math/Matrix4.hpp>

#include "Font.hpp"
#include "ScriptablePipeline.hpp"

namespace Hyper
{
	enum class DrawCallType
	{
		RectangleColored,
		RectangleMaterial,
		TextCharacter,
		BreakBatch,
		SetScissor,
		RectangleTextured,
		RectangleRounded
	};

	struct Render2DDrawCall
	{
		DrawCallType Type;
		int IndexOffset = 0;
		int IndexCount = 0;

		Rectangle Scissor;
		nvrhi::TextureHandle Texture;
	};

	struct Render2DObjectMetadata
	{
		//Used for rounded rect rendering
		struct RoundedRect
		{
			float Width;
			float Height;
			float Radius;
		} RoundedRect;
	};

	struct Vertex2D
	{
		Float2 Position;
		Float2 TexCoord;
		Color Tint;
		uint32 Metadata = 0;

		Vertex2D()
		{
		};

		Vertex2D(Float2 pos, Float2 uv) :
			Position(pos),
			TexCoord(uv)
		{

		}

		Vertex2D(Matrix4 transform, const Float2& offset, float x, float y, float uvX, float uvY, const Color& color) :
					Position(x, y), TexCoord(uvX, uvY), Tint(color)
		{
			const auto& transformed = Matrix4::Multiply(transform,Float4(Position.X, Position.Y, 0.0f, 1.0f));
			Position = offset + Float2(transformed.X, transformed.Y);
		}

		Vertex2D(float x, float y, float uvX, float uvY) :
			Vertex2D(Float2(x, y), Float2(uvX, uvY))
		{

		}

		Vertex2D(float x, float y, float uvX, float uvY, Color c, uint32 metadata = 0) :
			Vertex2D(Float2(x, y), Float2(uvX, uvY))
		{
			Tint = c;
			Metadata = metadata;
		}

		Vertex2D(Float2 pos, Float2 uv, Color color) :
			Position(pos),
			TexCoord(uv),
			Tint(color)
		{

		}
	};

	class HYPER_API Render2D
	{
	private:
		nvrhi::BufferHandle m_VB = nullptr;
		nvrhi::BufferHandle m_IB = nullptr;
		nvrhi::BufferHandle m_MetadataBuffer = nullptr;
		Matrix4 m_Projection;
		Matrix4 m_View;

		std::vector<Render2DObjectMetadata> m_Metadatas;

		nvrhi::InputLayoutHandle m_VertexLayout = nullptr;
		nvrhi::GraphicsPipelineDesc m_QuadPSO;
		nvrhi::BindingLayoutHandle m_QuadPSOLayout;
		nvrhi::BindingSetHandle m_QuadPSOSet;

		nvrhi::GraphicsPipelineDesc m_QuadTexturePSO;
		nvrhi::BindingLayoutHandle m_QuadTexturePSOLayout;

		nvrhi::GraphicsPipelineDesc m_QuadRoundedPSO;
		nvrhi::BindingLayoutHandle m_QuadRoundedPSOLayout;

		nvrhi::GraphicsPipelineDesc m_QuadFontPSO;
		nvrhi::BindingLayoutHandle m_QuadFontPSOLayout;

		nvrhi::SamplerHandle m_LinearSampler;

		std::vector<Render2DDrawCall> m_DrawCalls;
		std::vector<Matrix4> m_TransformStack;
		std::vector<Vertex2D> m_Vertices;
		std::vector<uint32> m_Indices;
		uint32 m_VertexOffset = 0;
		uint32 m_IndexOffset = 0;

		void DrawBatch(MinimalGraphicsContext& context, int32 batchStart, int32 batchSize);

	public:
		Render2D(nvrhi::DeviceHandle device);
		~Render2D() = default;

		void Begin(MinimalGraphicsContext& context);
		void DrawRect(const Rectangle& rect, const Color& c);
		void DrawRoundedRect(const Rectangle& rect, const Color& c, float rounding);
		void DrawTexture(const Rectangle& rect, nvrhi::TextureHandle texture);
		void DrawText(const String& text, Float2 position, const Color& color, Font& font, float scale, bool centeredX = false, bool centeredY = false);
		void Execute(MinimalGraphicsContext& context);
	};
}