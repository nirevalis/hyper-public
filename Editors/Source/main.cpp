#include <algorithm>
#include <cinttypes>
#include <iostream>
#include <fstream>
#include <ranges>
#include <string>
#include <unordered_map>
#include <Windows.h>
#include <filesystem>

#include <Asset/AssetRegistry.hpp>
#include <Renderer/Assets/TextureAsset.hpp>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "TinyGLTF/tiny_gltf.hpp"
#include "Lodepng/lodepng.hpp"

#include "Renderer/Material.hpp"
#include "Renderer/Assets/ModelAsset.hpp"

struct IEditorActivity
{
	std::string Name;
	std::string Description;

	IEditorActivity(std::string name, std::string desc) :
		Name(std::move(name)),
		Description(std::move(desc))
	{

	}

	IEditorActivity(const IEditorActivity&) = delete;
	IEditorActivity(IEditorActivity&&) = delete;

	virtual ~IEditorActivity() = default;

	static std::string AcceptString(const std::string& text)
	{
		std::string in;

		printf("%s -> ", text.c_str());
		std::getline(std::cin, in);

		return in;
	}

	virtual void Do() = 0;
};

struct ImportTextureActivity : public IEditorActivity
{
	ImportTextureActivity() : IEditorActivity("import_texture", "Creates a Hyper-compatible texture asset based on given image file.")
	{

	};

	void Do() override
	{
		const std::string& path = AcceptString("Path");
		const std::string& vfsPath = AcceptString("Virtual space path");

		Hyper::TextureAsset* asset = Hyper::AssetRegistry::CreateVirtual<Hyper::TextureAsset>(
			Hyper::AssetType::Texture, vfsPath.c_str()
		);

		Hyper::TextureAssetDescription& desc = asset->m_Description;


		int width, height;
		int channels;
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 4);

		if (data == nullptr)
		{
			printf("Decoder error\n");
			asset->Unload();
			return;
		}

		desc.Width = static_cast< int32 >(width);
		desc.Height = static_cast< int32 >(height);
		desc.Channels = 4;
		desc.Format = nvrhi::Format::RGBA8_UNORM;

		asset->m_Bytes = std::vector<unsigned char> { data ,  data + (width * height * 4) };

		std::filesystem::path desiredPath = path;
		desiredPath.replace_extension(".hpr");

		Hyper::AssetRegistry::Save(asset, desiredPath.string());

		stbi_image_free(data);
		asset->Unload();
		delete asset;
	}
};


struct ImportHDRActivity : public IEditorActivity
{
	ImportHDRActivity() : IEditorActivity("import_hdr", "Creates a Hyper-compatible hdr asset based on given hdr file.")
	{

	};

	void Do() override
	{
		const std::string& path = AcceptString("Path");
		const std::string& vfsPath = AcceptString("Virtual space path");

		Hyper::TextureAsset* asset = Hyper::AssetRegistry::CreateVirtual<Hyper::TextureAsset>(
			Hyper::AssetType::Texture, vfsPath.c_str()
		);

		Hyper::TextureAssetDescription& desc = asset->m_Description;


		int width, height;
		int channels;
		float* data = stbi_loadf(path.c_str(), &width, &height, &channels, 4);

		if (data == nullptr)
		{
			printf("Decoder error\n");
			asset->Unload();
			return;
		}

		desc.Width = static_cast< int32 >(width);
		desc.Height = static_cast< int32 >(height);
		desc.Channels = 4;
		desc.Format = nvrhi::Format::RGBA32_FLOAT;

		asset->m_HDRBytes = std::vector<float> { data ,  data + (width * height * 4) };

		std::filesystem::path desiredPath = path;
		desiredPath.replace_extension(".hpr");

		Hyper::AssetRegistry::Save(asset, desiredPath.string());

		stbi_image_free(data);
		asset->Unload();
		delete asset;
	}
};


struct ImportModelActivity : public IEditorActivity
{
	ImportModelActivity() : IEditorActivity("import_model_gltf", "Creates a Hyper-compatible model asset based on given GLTF file")
	{

	};

	void Do() override
	{
		const std::string& path = AcceptString("Path");
		const std::string& vfsPath = AcceptString("Virtual space path");

		Hyper::ModelAsset* asset = Hyper::AssetRegistry::CreateVirtual<Hyper::ModelAsset>(
			Hyper::AssetType::Model, vfsPath.c_str()
		);

		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err, warn;

		bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);
		if (!warn.empty()) INFO("Warn: {}", warn);
		if (!err.empty()) INFO("Error: {}", err);

		for (const auto& material : model.materials)
		{
			Hyper::Material hMaterial;

			hMaterial.Type = Hyper::SurfaceType::Solid;
			hMaterial.Color = Hyper::MaterialValue<Hyper::Float4>::FromValue({ ( float )material.pbrMetallicRoughness.baseColorFactor[0],( float )material.pbrMetallicRoughness.baseColorFactor[1], ( float )material.pbrMetallicRoughness.baseColorFactor[2], ( float )material.pbrMetallicRoughness.baseColorFactor[3] });
			hMaterial.Roughness = material.pbrMetallicRoughness.roughnessFactor;
			hMaterial.Metallic = material.pbrMetallicRoughness.metallicFactor;

			asset->m_Materials.push_back(hMaterial);
		}

		for (const auto& mesh : model.meshes)
		{
			for (const auto& primitive : mesh.primitives)
			{
				Hyper::MeshData data;
				if (primitive.indices < 0) continue;

				const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
				const tinygltf::BufferView& indexView = model.bufferViews[indexAccessor.bufferView];
				const tinygltf::Buffer& indexBuffer = model.buffers[indexView.buffer];

				const void* dataPtr = &indexBuffer.data[indexAccessor.byteOffset + indexView.byteOffset];
				size_t indexCount = indexAccessor.count;

				switch (indexAccessor.componentType)
				{
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
				{
					const uint16_t* buf = reinterpret_cast< const uint16_t* >(dataPtr);
					for (size_t i = 0; i < indexCount; ++i)
						data.Indices.push_back(static_cast< uint32_t >(buf[i]));
					break;
				}
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
				{
					const uint32_t* buf = reinterpret_cast< const uint32_t* >(dataPtr);
					for (size_t i = 0; i < indexCount; ++i)
						data.Indices.push_back(buf[i]);
					break;
				}
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
				{
					const uint8_t* buf = reinterpret_cast< const uint8_t* >(dataPtr);
					for (size_t i = 0; i < indexCount; ++i)
						data.Indices.push_back(static_cast< uint32_t >(buf[i]));
					break;
				}
				}

				const auto& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
				const auto& normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];

				tinygltf::Accessor* uvAccessor = nullptr;
				if (primitive.attributes.contains("TEXCOORD_0"))
					uvAccessor = &model.accessors[primitive.attributes.find("TEXCOORD_0")->second];

				const auto& posView = model.bufferViews[posAccessor.bufferView];
				const auto& normView = model.bufferViews[normAccessor.bufferView];

				const float* posData = reinterpret_cast< const float* >(
					&model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]);
				const float* normData = reinterpret_cast< const float* >(
					&model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]);
				const float* uvData = uvAccessor ? reinterpret_cast< const float* >(
					&model.buffers[model.bufferViews[uvAccessor->bufferView].buffer].data[uvAccessor->byteOffset + model.bufferViews[uvAccessor->bufferView].byteOffset]) : nullptr;

				for (size_t i = 0; i < posAccessor.count; ++i)
				{
					data.Vertices.emplace_back(
						Hyper::Float3(posData[i * 3 + 0], posData[i * 3 + 1], posData[i * 3 + 2]),
						uvData ? Hyper::Float2(uvData[i * 2 + 0], uvData[i * 2 + 1]) : Hyper::Float2(0,0),
						Hyper::Float3(normData[i * 3 + 0], normData[i * 3 + 1], normData[i * 3 + 2])
					);
				}

				data.Material = primitive.material;

				asset->m_MeshData.push_back(data);
			}
		}

		asset->m_Description.MeshCount = asset->m_MeshData.size();
		asset->m_Description.MaterialCount = asset->m_Materials.size();

		std::filesystem::path desiredPath = path;
		desiredPath.replace_extension(".hpr");

		Hyper::AssetRegistry::Save(asset, desiredPath.string());

		asset->Unload();
		delete asset;
	}
};

class EditorActivityRepository
{
private:
	std::unordered_map<std::string, IEditorActivity*> m_Activities;

public:
	~EditorActivityRepository()
	{
		for (IEditorActivity* activity : m_Activities | std::views::values)
			delete activity;
	}

	void Help() const
	{
		printf("=======================\n");
		for (IEditorActivity* activity : m_Activities | std::views::values)
		{
			printf("- %s - %s\n", activity->Name.c_str(), activity->Description.c_str());
		}
		printf("=======================\n");
	}

	void Add(IEditorActivity* activity)
	{
		m_Activities.emplace(activity->Name, activity);
	}

	void Execute(const std::string& text)
	{
		std::string loweredText;
		loweredText.resize(text.length());

		std::transform(text.begin(), text.end(), loweredText.begin(), ::tolower);

		if (loweredText == "h" || loweredText == "help")
		{
			Help();
			return;
		}

		if (m_Activities.contains(loweredText))
			m_Activities[loweredText]->Do();
	}
};

int32_t main()
{
	printf("Hyper CLI 1.0. Created by Piotr Chudzinski. All rights reserved\n");

	EditorActivityRepository repository;
	repository.Add(new ImportTextureActivity());
	repository.Add(new ImportModelActivity());
	repository.Add(new ImportHDRActivity());


	volatile static bool ShouldExit = false;
	while (!ShouldExit)
	{
		const auto& command = IEditorActivity::AcceptString("Command");
		if (command == "exit" || command == "e" || command == "EXIT")
			ShouldExit = true;

		repository.Execute(command);
	}

	return 0;
}