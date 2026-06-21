#pragma once

#include <Core/Common.hpp>
#include <unordered_map>
#include <nvrhi/nvrhi.h>

namespace Hyper
{
	struct ShaderInformation
	{
		String Name;
		String EntryPoint;
		nvrhi::ShaderType ShaderType;

		uint64 HashCode() const;
	};

	class HYPER_API ShaderLibrary
	{
	private:
		static std::unordered_map<uint64, nvrhi::ShaderHandle> m_Shaders;

	public:
		static void LoadFrom(nvrhi::DeviceHandle device, const String& path);
		static nvrhi::ShaderHandle GetShader(ShaderInformation& info);
	};
}