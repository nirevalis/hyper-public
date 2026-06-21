#pragma once

#include <Core/Common.hpp>
#include <Core/Math/Float4.hpp>

namespace Hyper
{
#define SURFACE_SOLID 0

	enum class SurfaceType : uint32
	{
		Solid = SURFACE_SOLID
	};

	template <typename TBase>
	struct MaterialValue
	{
		TBase Value;
		nvrhi::TextureHandle Map;
		bool UseMap = false;

		constexpr static MaterialValue FromValue(const TBase& base)
		{
			MaterialValue material;
			material.Value = base;
			material.Map = nullptr;
			material.UseMap = false;

			return material;
		}

		constexpr static MaterialValue FromMap(nvrhi::TextureHandle handle)
		{
			MaterialValue material;
			material.Map = handle;
			material.UseMap = true;

			return material;
		}
	};

	struct Material
	{
		SurfaceType Type;

		//For Solid
		MaterialValue<Float4> Color;
		MaterialValue<Float4> Normal;
		float Roughness = 0;
		float Metallic = 0;

		Material()
		{

		}
	};
}
