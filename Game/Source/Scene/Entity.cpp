#include "Entity.hpp"

namespace Hyper
{
	Guid Entity::GetId()
	{
		return m_Id;
	}

	RadiansTransform& Entity::GetTransform()
	{
		return m_Transform;
	}

	EntityMobility Entity::GetMobility()
	{
		return EntityMobility::Static;
	}
}