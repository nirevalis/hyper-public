#pragma once

#include <unordered_map>
#include <Core/Common.hpp>
#include <nvrhi/nvrhi.h>

namespace Hyper
{
	class DescriptorTableManager;
	typedef int32 DescriptorIndex;

	class DescriptorHandle
	{
	private:
		DescriptorTableManager* m_Manager;
		DescriptorIndex m_DescriptorIndex;

	public:
		DescriptorHandle();
		DescriptorHandle(DescriptorTableManager* manager, DescriptorIndex index);
		~DescriptorHandle();

		[[nodiscard]] bool IsValid() const
		{
			return m_DescriptorIndex >= 0;
		}

		[[nodiscard]] DescriptorIndex Get() const
		{
			return m_DescriptorIndex;
		}

		[[nodiscard]] DescriptorIndex GetIndexInHeap() const;

		void Reset()
		{
			m_DescriptorIndex = -1;
		}

		DescriptorHandle(const DescriptorHandle&) = delete;
		DescriptorHandle(DescriptorHandle&&) = default;
		DescriptorHandle& operator=(const DescriptorHandle&) = delete;
		DescriptorHandle& operator=(DescriptorHandle&&) = default;
	};

	class DescriptorTableManager : public std::enable_shared_from_this<DescriptorTableManager>
	{
	protected:
		struct BindingSetItemHasher
		{
			std::size_t operator()(const nvrhi::BindingSetItem& item) const
			{
				size_t hash = 0;
				nvrhi::hash_combine(hash, item.resourceHandle);
				nvrhi::hash_combine(hash, item.type);
				nvrhi::hash_combine(hash, item.format);
				nvrhi::hash_combine(hash, item.dimension);
				nvrhi::hash_combine(hash, item.rawData[0]);
				nvrhi::hash_combine(hash, item.rawData[1]);
				return hash;
			}
		};

		struct BindingSetItemsEqual
		{
			bool operator()(const nvrhi::BindingSetItem& a, const nvrhi::BindingSetItem& b) const
			{
				return a.resourceHandle == b.resourceHandle
					&& a.type == b.type
					&& a.format == b.format
					&& a.dimension == b.dimension
					&& a.subresources == b.subresources;
			}
		};

		nvrhi::DeviceHandle m_Device;
		nvrhi::DescriptorTableHandle m_DescriptorTable;

		std::vector<nvrhi::BindingSetItem> m_Descriptors;
		std::unordered_map<nvrhi::BindingSetItem, DescriptorIndex, BindingSetItemHasher, BindingSetItemsEqual> m_DescriptorIndexMap;
		std::vector<bool> m_AllocatedDescriptors;
		int32 m_SearchStart = 0;

	public:
		DescriptorTableManager(nvrhi::IDevice* device, nvrhi::IBindingLayout* layout);
		~DescriptorTableManager();

		nvrhi::IDescriptorTable* GetDescriptorTable() const
		{
			return m_DescriptorTable;
		}

		DescriptorIndex CreateDescriptor(nvrhi::BindingSetItem item);
		DescriptorHandle CreateDescriptorHandle(nvrhi::BindingSetItem item);
		nvrhi::BindingSetItem GetDescriptor(DescriptorIndex index);
		void ReleaseDescriptor(DescriptorIndex index);
	};
}