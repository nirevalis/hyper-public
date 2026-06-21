#pragma once

#include <nvrhi/nvrhi.h>
#include <Core/String/String.hpp>

namespace Hyper
{
	class HYPER_API Skybox
	{
	private:
		nvrhi::TextureHandle m_Irradiance;
		nvrhi::TextureHandle m_Radiance;
	public:
		Skybox(const String& skyboxLocation);

		nvrhi::TextureHandle GetIrradiance();
		nvrhi::TextureHandle GetRadiance();
	};
}
