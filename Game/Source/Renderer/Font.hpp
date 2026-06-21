#pragma once

#include <Core/Common.hpp>
#include <msdf-atlas-gen/GlyphGeometry.h>
#include <msdf-atlas-gen/FontGeometry.h>
#include <nvrhi/nvrhi.h>

namespace Hyper
{
    class Font
    {
    public:
        std::vector<msdf_atlas::GlyphGeometry> Glyphs;
        msdf_atlas::FontGeometry Geometry;
        nvrhi::TextureHandle Atlas;

        Font(const String& path);
        ~Font() = default;

        float GetWidth(const String& text, float size);
    };
}
