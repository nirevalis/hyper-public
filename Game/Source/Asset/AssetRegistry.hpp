#pragma once

#include <unordered_map>
#include <Core/Common.hpp>

#include "Asset.hpp"

namespace Hyper
{
	class HYPER_API AssetRegistry
	{
	private:
		inline static std::unordered_map<Guid, Asset*> m_Assets;
		inline static std::unordered_map<String, Asset*> m_VirtualPathes;
		static Asset* createForType(const AssetType type);

	public:
		template <typename TCast>
		inline static TCast* CreateVirtual(const AssetType type, const String& virtualPath)
		{
			Asset* asset = createForType(type);
			asset->m_IsVirtual = true;
			asset->m_VirtualPath = virtualPath;
			asset->m_Id = Guid::Random();

			m_Assets.emplace(asset->GetId(), asset);
			INFO("Created virtual asset '{}'.", asset->GetId().ToString());

			return dynamic_cast< TCast* >(asset);
		}

		static Asset* LoadAndFlush(const String& systemPath);
		static Asset* Get(const String& virtualPath);

		template <typename TAsset>
		static TAsset* GetAs(const String& virtualPath)
		{
			return ( TAsset* )Get(virtualPath);
		}

		static void Save(Asset* asset, const String& systemPath);
	};
}
