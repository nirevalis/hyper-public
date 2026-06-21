#include "ModelAsset.hpp"
#include <nvrhi/nvrhi.h>
#include "Core/Serialization/Stream.hpp"
#include "Engine/GameEngine.hpp"

namespace Hyper
{
	namespace Internal
	{
		static int32 GetChannelCount(nvrhi::Format format)
		{
			switch (format)
			{
			case nvrhi::Format::R8_UNORM:
				return 1;
			case nvrhi::Format::RG8_UNORM:
				return 2;
			case nvrhi::Format::RGB32_SINT:
				return 3;
			case nvrhi::Format::RGBA8_UNORM:
				return 4;
			default:
				ERR("Unknown format!");
				return 0;
			}
		}
	}

	void ModelAsset::FlushObjects()
	{
		ModelFlushedData* data = new ModelFlushedData();

		data->Materials = m_Materials;

		for (const MeshData& mesh : m_MeshData)
		{
			data->Meshes.emplace_back(mesh.Vertices, mesh.Indices, data->Materials[mesh.Material]);
		}

		uploadFlushedObjects(data);
	}

	template <typename T>
	static void SerializeMaterialValue(std::ostream& stream, MaterialValue<T> value)
	{
		stream.write(reinterpret_cast< const char* >(&value.UseMap), sizeof(bool));
		stream.write(reinterpret_cast< const char* >(&value.Value), sizeof(T));
	}

	template <typename T>
	static MaterialValue<T> LoadMaterialValue(std::istream& stream)
	{
		MaterialValue<T> value;

		stream.read(reinterpret_cast< char* >(&value.UseMap), sizeof(bool));
		stream.read(reinterpret_cast< char* >(&value.Value), sizeof(T));

		return value;
	}

	void ModelAsset::Load(std::istream& stream)
	{
		Asset::Load(stream);

		stream.read(reinterpret_cast< char* >(&m_Description), sizeof(ModelAssetDescription));

		for (int32 i = 0; i < m_Description.MeshCount; i++)
		{
			MeshData data = {};

			data.Vertices.resize(Stream::ReadUInt32(stream));
			stream.read(reinterpret_cast< char* >(data.Vertices.data()), data.Vertices.size() * sizeof(Vertex3D));

			data.Indices.resize(Stream::ReadUInt32(stream));
			stream.read(reinterpret_cast< char* >(data.Indices.data()), data.Indices.size() * sizeof(uint32));

			data.Material = Stream::ReadInt32(stream);

			m_MeshData.emplace_back(data);
		}

		for (int32 i = 0; i < m_Description.MaterialCount; i++)
		{
			Material material;

			material.Type = static_cast< SurfaceType >(Stream::ReadUInt32(stream));
			material.Color = LoadMaterialValue<Float4>(stream);
			material.Normal = LoadMaterialValue<Float4>(stream);
			material.Roughness = Stream::ReadFloat(stream);
			material.Metallic = Stream::ReadFloat(stream);

			m_Materials.push_back(material);
		}
	}

	void ModelAsset::Save(std::ostream& stream)
	{
		bool unloadAfter = false;

		if (!isMemoryAccessible())
		{
			unloadAfter = true;
			// TODO: try to load asset to the memory.
		}

		stream.write(reinterpret_cast< const char* >(&m_Description), sizeof(ModelAssetDescription));

		for (const MeshData& data : m_MeshData)
		{
			Stream::WriteUInt32(stream, data.Vertices.size());
			stream.write(reinterpret_cast< const char* >(data.Vertices.data()), data.Vertices.size() * sizeof(Vertex3D));

			Stream::WriteUInt32(stream, data.Indices.size());
			stream.write(reinterpret_cast< const char* >(data.Indices.data()), data.Indices.size() * sizeof(uint32));

			Stream::WriteInt32(stream, data.Material);
		}

		for (const Material& material : m_Materials)
		{
			Stream::WriteUInt32(stream, static_cast< uint32 >(material.Type));
			SerializeMaterialValue(stream, material.Color);
			SerializeMaterialValue(stream, material.Normal);
			Stream::WriteFloat(stream, material.Roughness);
			Stream::WriteFloat(stream, material.Metallic);
		}

		if (unloadAfter)
		{
			// TODO: add unloading
		}
	}

	void ModelAsset::Unload()
	{
		Asset::Unload();

		if (!m_MeshData.empty())
			m_MeshData.clear();

		if (!m_Materials.empty())
			m_Materials.clear();
	}

	ModelFlushedData* ModelAsset::GetModelData() const
	{
		return reinterpret_cast< ModelFlushedData* >(m_FlushedHandle);
	}

	ModelAssetDescription ModelAsset::GetDescription() const
	{
		return m_Description;
	}
}