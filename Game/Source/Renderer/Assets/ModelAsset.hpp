#pragma once

#include <Core/Common.hpp>
#include <Asset/Asset.hpp>
#include <nvrhi/nvrhi.h>

#include "Renderer/Mesh.hpp"
#include <Renderer/Material.hpp>

struct ImportModelActivity;

namespace Hyper
{
	struct HYPER_API ModelAssetDescription
	{
		int32 MeshCount;
		int32 MaterialCount;

		ModelAssetDescription() : MeshCount(0), MaterialCount(0)
		{

		}
	};

	struct HYPER_API MeshData
	{
		std::vector<Vertex3D> Vertices;
		std::vector<uint32> Indices;
		int32 Material;
	};

	struct HYPER_API ModelFlushedData
	{
		std::vector<Mesh> Meshes;
		std::vector<Material> Materials;
	};

	class HYPER_API ModelAsset : public Asset
	{
	private:
		friend struct ::ImportModelActivity;

		ModelAssetDescription m_Description;
		std::vector<MeshData> m_MeshData;
		std::vector<Material> m_Materials;

	public:
		ModelAsset() : Asset(AssetType::Model)
		{
		}

		~ModelAsset() override = default;
		ModelAsset(const ModelAsset&) = delete;

		void FlushObjects() override;
		void Save(std::ostream& stream) override;
		void Load(std::istream& stream) override;
		void Unload() override;

		ModelFlushedData* GetModelData() const;
		ModelAssetDescription GetDescription() const;
	};
}
