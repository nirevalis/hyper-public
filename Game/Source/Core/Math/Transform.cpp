#include "Transform.hpp"

namespace Hyper
{
	void RadiansTransform::Move(Float3 position)
	{
		Position = Position + position;
	}

	void RadiansTransform::Rotate(Quaternion orientation)
	{
		Orientation = Orientation + orientation;
	}

	Float3 RadiansTransform::GetForward()
	{
		return Orientation * Float3(0, 0, 1);
	}

	Float3 RadiansTransform::GetUp()
	{
		return Float3::Cross(GetForward(), GetRight());
	}

	Float3 RadiansTransform::GetRight()
	{
		return Orientation * Float3(1, 0, 0);
	}

	void EulerTransform::Move(Float3 position)
	{
		Position = Position + position;
	}

	void EulerTransform::Rotate(Float3 orientation)
	{
		Orientation = Orientation + orientation;
	}

	Float3 EulerTransform::GetForward()
	{
		Float3 ori = Orientation;
		ori = Float3(Math::Radians(ori.X), Math::Radians(ori.Y), Math::Radians(ori.Z));
		return Float3(Math::Cos(ori.X) * Math::Sin(ori.Y), -Math::Sin(ori.X), Math::Cos(ori.X) * Math::Cos(ori.Y));
	}

	Float3 EulerTransform::GetUp()
	{
		return Float3::Cross(GetForward(), GetRight());
	}

	Float3 EulerTransform::GetRight()
	{
		Float3 ori = Orientation;
		ori = Float3(Math::Radians(ori.X), Math::Radians(ori.Y), Math::Radians(ori.Z));
		return Float3(Math::Cos(ori.Y), 0, -Math::Sin(ori.Y));
	}
}