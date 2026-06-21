#pragma once

#include "Mesh.hpp"
#include "Core/Math/Transform.hpp"
#include "Material.hpp"
#include <vector>

namespace Hyper
{
	struct HYPER_API DrawCall
	{
	public:
		Mesh* Mesh;
		RadiansTransform Transform;
		Material Material;
	};

	class HYPER_API DrawCallCollectContext
	{
	private:
		friend class SceneRenderer;
		std::vector<DrawCall> m_DrawCalls;

	public:
		void Add(const DrawCall& drawCall)
		{
			m_DrawCalls.push_back(drawCall);
		}
	};
}
