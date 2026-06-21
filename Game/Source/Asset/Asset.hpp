#pragma once

#include <Core/Common.hpp>
#include <iostream>
#include <Core/GUID.hpp>

namespace Hyper
{
	enum class AssetType : uint8
	{
		Texture = 0,
		Font = 1,
		Model = 2
	};

	class HYPER_API Asset
	{
	private:
		friend class AssetRegistry;

		Guid m_Id;
		String m_OSPath;
		String m_VirtualPath;
		AssetType m_Type;
		bool m_IsVirtual = false;
		bool m_IsLoadedInMemory = false;

	protected:
		void* m_FlushedHandle = nullptr;

		bool isMemoryAccessible() const
		{
			return m_IsLoadedInMemory;
		}

		void uploadFlushedObjects(void* obj)
		{
			m_FlushedHandle = obj;
		}

	public:
		explicit Asset(const AssetType type) : m_Type(type)
		{

		}

		virtual ~Asset() = default;

		Asset(const Asset&) = delete;

		[[nodiscard]] AssetType GetType() const;
		[[nodiscard]] Guid GetId() const;
		[[nodiscard]] bool IsVirtual() const;

		virtual void Unload()
		{
			m_IsLoadedInMemory = false;
		}

		virtual void FlushObjects() = 0;
		virtual void Save(std::ostream& stream) = 0;

		virtual void Load(std::istream& stream)
		{
			m_IsLoadedInMemory = true;
		}
	};
}