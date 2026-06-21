#include "AssetRegistry.hpp"
#include <Core/Serialization/Stream.hpp>
#include <fstream>

#include <Renderer/Assets/TextureAsset.hpp>

#include "Renderer/Assets/ModelAsset.hpp"

namespace Hyper
{
	//std::unordered_map<Guid, Asset*> AssetRegistry::m_Assets;

	Asset* AssetRegistry::createForType(const AssetType type)
	{
		switch (type)
		{
		case AssetType::Texture:
			return new TextureAsset();
		case AssetType::Model:
			return new ModelAsset();
		default:
			ERR("Unknown asset type '{}'.", static_cast< int32 >(type));
			return nullptr;
		}
	}

	Asset* AssetRegistry::LoadAndFlush(const String& systemPath)
	{
		std::ifstream f(systemPath.Get(), std::ios::binary);

		AssetType type = Stream::ReadEnum<AssetType>(f);
		Asset* asset = createForType(type);

		asset->m_Id = Stream::ReadGuid(f);
		asset->m_VirtualPath = Stream::ReadString(f);
		asset->Load(f);

		asset->FlushObjects();
		asset->Unload();

		f.close();

		m_Assets.emplace(asset->GetId(), asset);
		m_VirtualPathes.emplace(asset->m_VirtualPath, asset);
		INFO("Loaded '{}' as '{}'.", systemPath, asset->GetId().ToString());

		return asset;
	}

	void AssetRegistry::Save(Asset* asset, const String& systemPath)
	{
		std::ofstream f(systemPath.Get(), std::ios::binary);
		Stream::WriteEnum<AssetType>(f, asset->GetType());
		Stream::WriteGuid(f, asset->GetId());
		Stream::WriteString(f, asset->m_VirtualPath);
		asset->Save(f);

		f.close();
	}

	Asset* AssetRegistry::Get(const String& virtualPath)
	{
		return m_VirtualPathes[virtualPath];
	}

}
