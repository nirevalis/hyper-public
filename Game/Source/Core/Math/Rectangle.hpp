#pragma once

#include <Core/Common.hpp>
#include "Float2.hpp"

namespace Hyper
{
	struct HYPER_API Rectangle
	{
	private:
		void ResetConsts()
		{
			Center = Float2(X, Y) + Float2(Width, Height) * 0.5f;
		}
	public:
		const static Rectangle Zero;

		float X;
		float Y;
		float Width;
		float Height;

		Float2 Center;

		Rectangle() : X(0), Y(0), Width(0), Height(0)
		{
			ResetConsts();
		};
		Rectangle(float x, float y, float width, float height) : X(x), Y(y), Width(width), Height(height)
		{
			ResetConsts();
		};
		Rectangle(Float2 position, Float2 size) : X(position.X), Y(position.Y), Width(size.X), Height(size.Y)
		{
			ResetConsts();
		};
	};
}