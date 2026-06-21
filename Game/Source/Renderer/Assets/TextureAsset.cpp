#include "TextureAsset.hpp"
#include <nvrhi/nvrhi.h>
#include "Core/Serialization/Stream.hpp"
#include "Engine/GameEngine.hpp"
#include <Renderer/Async/TextureUploadTask.hpp>
#include "Threading/JobManager.hpp"

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

	void TextureAsset::FlushObjects()
	{
		if (m_Bytes.empty() && m_HDRBytes.empty())
		{
			ERR("m_Bytes && m_HDRBytes is empty");
			return;
		}

		auto textureDesc = nvrhi::TextureDesc()
			.setDimension(nvrhi::TextureDimension::Texture2D)
			.setWidth(m_Description.Width)
			.setHeight(m_Description.Height)
			.setFormat(m_Description.Format)
			.setInitialState(nvrhi::ResourceStates::ShaderResource)
			.setKeepInitialState(true)
			.setDebugName(GetId().ToString().Get());

		DeviceHelper& helper = GameEngine::Get().GetDeviceHelper();
		nvrhi::TextureHandle texture = helper.GetDevice()->createTexture(textureDesc);

		ITextureUploadTaskBase* task = nullptr;
		TaskGraph graph;

		if (m_HDRBytes.empty())
			graph.AddTask(new TextureUploadTask<unsigned char, 4>(m_Bytes, texture), ( void** )&task);
		else
			graph.AddTask(new TextureUploadTask<float, 16>(m_HDRBytes, texture), ( void** )&task);

		//TODO: add mipmap generation to taskgraph.

		graph.AddOnFinished([this, task] ()
			{
				INFO("[ASYNC] Uploaded texture '{}' with {}x{} size in {}",
					GetId().ToString().Get(),
					task->Texture->getDesc().width,
					task->Texture->getDesc().height,
					task->GetTaskDuration());
				delete task;
			});

		JobManager::Run(TaskImportance::Side, graph);

		texture->AddRef();
		uploadFlushedObjects(texture);
	}

	void TextureAsset::Load(std::istream& stream)
	{
		Asset::Load(stream);

		stream.read(reinterpret_cast< char* >(&m_Description), sizeof(TextureAssetDescription));

		m_Bytes.resize(Stream::ReadInt64(stream));
		stream.read(reinterpret_cast< char* >(m_Bytes.data()), m_Bytes.size());

		m_HDRBytes.resize(Stream::ReadInt64(stream));
		if (!m_HDRBytes.empty())
			stream.read(reinterpret_cast< char* >(m_HDRBytes.data()), m_HDRBytes.size() * sizeof(float));
	}

	void TextureAsset::Save(std::ostream& stream)
	{
		bool unloadAfter = false;

		if (!isMemoryAccessible())
		{
			unloadAfter = true;
			// TODO: try to load asset to the memory.
		}

		stream.write(reinterpret_cast< const char* >(&m_Description), sizeof(TextureAssetDescription));

		Stream::WriteInt64(stream, m_Bytes.size());
		stream.write(reinterpret_cast< const char* >(m_Bytes.data()), m_Bytes.size());

		Stream::WriteInt64(stream, m_HDRBytes.size());
		if (!m_HDRBytes.empty())
			stream.write(reinterpret_cast< const char* >(m_HDRBytes.data()), m_HDRBytes.size() * sizeof(float));

		if (unloadAfter)
		{
			// TODO: add unloading
		}
	}

	void TextureAsset::Unload()
	{
		Asset::Unload();

		if (!m_Bytes.empty())
			m_Bytes.clear();

		if (!m_HDRBytes.empty())
			m_HDRBytes.clear();
	}

	nvrhi::TextureHandle TextureAsset::GetTexture() const
	{
		return ( nvrhi::ITexture* )m_FlushedHandle;
	}

	TextureAssetDescription TextureAsset::GetDescription() const
	{
		return m_Description;
	}


}
