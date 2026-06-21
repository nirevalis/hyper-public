#pragma once

#include <Core/Common.hpp>
#include <Core/Math/Transform.hpp>

#include "Core/Math/Matrix4.hpp"

namespace Hyper
{
	class HYPER_API Camera
	{
	private:
		EulerTransform m_Transform;
	public:
		Camera() = default;
		~Camera() = default;

		EulerTransform& GetTransform();
	};
}
