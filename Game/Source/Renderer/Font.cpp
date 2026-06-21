#include "Font.hpp"

#include <Engine/GameEngine.hpp>
#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <Threading/JobManager.hpp>

#include "Async/TextureUploadTask.hpp"

#define LCG_MULTIPLIER 6364136223846793005ull
#define LCG_INCREMENT 1442695040888963407ull
#define DEFAULT_ANGLE_THRESHOLD 3.0

namespace Hyper
{
    Font::Font(const String& path) : Geometry(&Glyphs)
    {
        msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();

        msdfgen::FontHandle* fontHandle = msdfgen::loadFont(ft, path.Get());

		int loadCount = Geometry.loadCharset(fontHandle, 1.0, msdf_atlas::Charset::ASCII);

		msdf_atlas::TightAtlasPacker packer;
		packer.setPixelRange(4.0);
		packer.setMiterLimit(1.0);
		packer.setScale(40);
		int32 remaining = packer.pack(Glyphs.data(), Glyphs.size());

		int32 width, height;
		packer.getDimensions(width, height);

		INFO("Packed {} of {} characters into atlas", (loadCount - remaining), loadCount);

		uint64 coloringSeed = 0;

		msdf_atlas::Workload([&glyphs = Glyphs, &coloringSeed] (int i, int threadNo) -> bool
			{
				unsigned long long glyphSeed = (LCG_MULTIPLIER * (coloringSeed ^ i) + LCG_INCREMENT);
				glyphs[i].edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyphSeed);
				return true;
			}, Glyphs.size()).finish(8);

		msdf_atlas::GeneratorAttributes attributes;
		attributes.config.overlapSupport = true;
		attributes.scanlinePass = true;

		msdf_atlas::ImmediateAtlasGenerator<float, 4, msdf_atlas::mtsdfGenerator, msdf_atlas::BitmapAtlasStorage<float, 4>> gen(width, height);
		gen.setAttributes(attributes);
		gen.setThreadCount(8);
		gen.generate(Glyphs.data(), Glyphs.size());

		msdfgen::BitmapConstRef<float, 4> bitmap = (msdfgen::BitmapConstRef<float, 4>)gen.atlasStorage();

    	auto textureDesc = nvrhi::TextureDesc()
					.setDimension(nvrhi::TextureDimension::Texture2D)
					.setWidth(bitmap.width)
					.setHeight(bitmap.height)
					.setFormat(nvrhi::Format::RGBA32_FLOAT)
					.setInitialState(nvrhi::ResourceStates::ShaderResource)
					.setKeepInitialState(true)
					.setDebugName("Font Atlas");

		Atlas = GameEngine::Get().GetDeviceHelper().GetDevice()->createTexture(textureDesc);

    	ITextureUploadTaskBase* task = nullptr;
    	TaskGraph graph;

    	int32 length = bitmap.width * bitmap.height * 4;
    	graph.AddTask(new TextureUploadTask<float, 16>({bitmap.pixels, bitmap.pixels + length}, Atlas), ( void** )&task);

    	graph.AddOnFinished([this, task] ()
			{
				INFO("[ASYNC] Uploaded font atlas with {}x{} size in {}",
					task->Texture->getDesc().width,
					task->Texture->getDesc().height,
					task->GetTaskDuration());
				delete task;
			});

    	JobManager::Run(TaskImportance::Side, graph);

        msdfgen::deinitializeFreetype(ft);
    }

	float Font::GetWidth(const String &text, float size)
	{
    	const auto& fontGeometry = Geometry;
		const auto& metrics = fontGeometry.getMetrics();

		double x = 0;
		double fsScale = (1.0 / (metrics.ascenderY - metrics.descenderY));
		double y = -metrics.lineHeight;

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
				return -1;

			if (i < text.GetLength() - 1)
			{
				double advance = glyph->getAdvance();
				char nextCharacter = text.Get()[i + 1];
				fontGeometry.getAdvance(advance, character, nextCharacter);

				x += fsScale * advance;
			}
		}

    	return x * size;
	}
}
