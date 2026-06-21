#include "Matrix4.hpp"
#include "Float3.hpp"
#include "Matrix3.hpp"
#include "Rectangle.hpp"

namespace Hyper
{
	const Matrix4 Matrix4::Zero(
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0
	);

	const Matrix4 Matrix4::Identity(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);

	const Float2 Float2::One = Float2(1, 1);
	const Float2 Float2::Zero = Float2(0, 0);

	const Float3 Float3::One = Float3(1, 1, 1);
	const Float3 Float3::Zero = Float3(0, 0, 0);

	const Float4 Float4::One = Float4(1, 1, 1, 1);
	const Float4 Float4::Zero = Float4(0, 0, 0, 0);

	const Rectangle Rectangle::Zero = Rectangle(0, 0, 0, 0);
}