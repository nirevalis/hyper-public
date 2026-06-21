#include "Asset.hpp"

namespace Hyper
{
	AssetType Asset::GetType() const
	{
		return m_Type;
	}

	bool Asset::IsVirtual() const
	{
		return m_IsVirtual;
	}

	Guid Asset::GetId() const
	{
		return m_Id;
	}
}