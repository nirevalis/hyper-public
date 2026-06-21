#pragma once

#include <Core/Common.hpp>
#include "Float4.hpp"

namespace Hyper
{
	class HYPER_API Color
	{
	public:
		union
		{
			struct
			{
				float Red, Green, Blue, Alpha;
			};

			float Raw[4];
		};

		Color() : Red(0), Green(0), Blue(0), Alpha(0)
		{

		}

		static Color FromRGBA(int red, int green, int blue, int alpha)
		{
			Color color;
			color.Red = red / 255.0f;
			color.Green = green / 255.0f;
			color.Blue = blue / 255.0f;
			color.Alpha = alpha / 255.0f;
			return color;
		}

		static Color FromRGB(int red, int green, int blue)
		{
			Color color;
			color.Red = red / 255.0f;
			color.Green = green / 255.0f;
			color.Blue = blue / 255.0f;
			color.Alpha = 1;
			return color;
		}

		static Color FromRGBA(float red, float green, float blue, float alpha)
		{
			Color color;
			color.Red = red;
			color.Green = green;
			color.Blue = blue;
			color.Alpha = alpha;
			return color;
		}

		static Color FromRGB(float red, float green, float blue)
		{
			Color color;
			color.Red = red;
			color.Green = green;
			color.Blue = blue;
			color.Alpha = 1;
			return color;
		}

		Float4 ToVector()
		{
			return Float4(Red, Green, Blue, Alpha);
		}

		Float3 ToVector3()
		{
			return Float3(Red, Green, Blue);
		}
	};
}