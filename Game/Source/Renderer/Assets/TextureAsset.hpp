#pragma once

#include <Core/Common.hpp>
#include <Asset/Asset.hpp>
#include <nvrhi/nvrhi.h>

struct ImportTextureActivity;
struct ImportHDRActivity;

namespace Hyper
{
	struct HYPER_API TextureAssetDescription
	{
		int32 Width;
		int32 Height;
		int32 Channels;
		nvrhi::Format Format;

		TextureAssetDescription() : Width(0), Height(0), Channels(0), Format(nvrhi::Format::RGBA8_UNORM)
		{

		}
	};

	class HYPER_API TextureAsset : public Asset
	{
	private:
		friend struct ::ImportTextureActivity;
		friend struct ::ImportHDRActivity;

		TextureAssetDescription m_Description;
		std::vector<unsigned char> m_Bytes;
		std::vector<float> m_HDRBytes;
	public:
		TextureAsset() : Asset(AssetType::Texture)
		{
		}

		~TextureAsset() override = default;
		TextureAsset(const TextureAsset&) = delete;

		void FlushObjects() override;
		void Save(std::ostream& stream) override;
		void Load(std::istream& stream) override;
		void Unload() override;

		nvrhi::TextureHandle GetTexture() const;
		TextureAssetDescription GetDescription() const;
	};
}
