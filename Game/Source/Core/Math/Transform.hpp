#pragma once
#include <Core/Common.hpp>
#include "Float3.hpp"
#include "Quaternion.hpp"

namespace Hyper
{
	struct HYPER_API RadiansTransform
	{
	public:
		Float3 Position = Float3::Zero;
		Quaternion Orientation = Quaternion::Euler(Float3(0,0,0));
		Float3 Scale = Float3::One;

		void Move(Float3 position);
		void Rotate(Quaternion orientation);

		Float3 GetForward();
		Float3 GetUp();
		Float3 GetRight();
	};

	struct HYPER_API EulerTransform
	{
	public:
		Float3 Position = Float3::Zero;
		Float3 Orientation = Float3::Zero;
		Float3 Scale = Float3::One;

		void Move(Float3 position);
		void Rotate(Float3 orientation);

		Float3 GetForward();
		Float3 GetUp();
		Float3 GetRight();
	};
}