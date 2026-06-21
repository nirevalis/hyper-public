#include "ShaderLibrary.hpp"
#include <filesystem>
#include <ranges>
#include <fstream>

namespace Hyper
{
	std::unordered_map<uint64, nvrhi::ShaderHandle> ShaderLibrary::m_Shaders;

	namespace Internal
	{
		std::pair<std::unique_ptr<char[]>, size_t> LoadFileToBuffer(const std::filesystem::path& filePath)
		{
			std::ifstream file(filePath, std::ios::binary | std::ios::ate);
			if (!file)
			{
				throw std::runtime_error("Failed to open file: " + filePath.string());
			}

			std::streamsize size = file.tellg();
			file.seekg(0, std::ios::beg);

			std::unique_ptr<char[]> buffer(new char[size]);
			if (!file.read(buffer.get(), size))
			{
				throw std::runtime_error("Failed to read file: " + filePath.string());
			}

			return { std::move(buffer), static_cast< size_t >(size) };
		}
	}

	uint64 ShaderInformation::HashCode() const
	{
		std::size_t hash = 0;

		auto hash_combine = [] (std::size_t& seed, std::size_t value)
			{
				seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			};

		hash_combine(hash, Name.HashCode());
		hash_combine(hash, static_cast< int32 >(ShaderType));

		return hash;
	}

	void ShaderLibrary::LoadFrom(nvrhi::DeviceHandle device, const String& path)
	{
		std::filesystem::path p(path.Get());

		for (const auto& entry : std::filesystem::directory_iterator(path.Get()))
		{
			if (!entry.is_regular_file())
				continue;

			const std::string& filePath = entry.path().string();
			const std::string& fileName = entry.path().filename().string();

			auto splitRange = fileName
				| std::views::split('_')
				| std::views::transform([] (auto&& subrange)
					{
						return std::string(subrange.begin(), subrange.end());
					});

			std::vector<std::string> split(splitRange.begin(), splitRange.end());
			if (split.size() < 2)
				continue;

			ShaderInformation information {};
			information.Name = split[0];
			information.EntryPoint = std::format("{}_Main", split[1]);

			switch (XORStr(split[1]))
			{
			case XORStr("VS"): information.ShaderType = nvrhi::ShaderType::Vertex; break;
			case XORStr("PS"): information.ShaderType = nvrhi::ShaderType::Pixel; break;
			case XORStr("CS"): information.ShaderType = nvrhi::ShaderType::Compute; break;
			case XORStr("RG"): information.ShaderType = nvrhi::ShaderType::RayGeneration; break;
			case XORStr("MI"): information.ShaderType = nvrhi::ShaderType::Miss; break;
			case XORStr("CL"): information.ShaderType = nvrhi::ShaderType::ClosestHit; break;
			}

			nvrhi::ShaderDesc desc {};
			desc.setDebugName(information.Name.Get());
			desc.setShaderType(information.ShaderType);
			desc.setEntryName(information.EntryPoint.Get());

			const auto& pair = Internal::LoadFileToBuffer(entry);

			INFO("Creating shader {} ({}).", information.Name, split[1]);
			m_Shaders[information.HashCode()] = device->createShader(desc, pair.first.get(), pair.second);
		}
	}

	nvrhi::ShaderHandle ShaderLibrary::GetShader(ShaderInformation& info)
	{
		const uint64 hash = info.HashCode();

		if (!m_Shaders.contains(hash))
			return nullptr;

		return m_Shaders.at(hash);
	}
}
