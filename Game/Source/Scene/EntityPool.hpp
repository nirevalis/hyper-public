#pragma once

#include <Core/Common.hpp>
#include <vector>

namespace Hyper
{
	template<typename T, int32 PoolSize = 1024>
	class HYPER_API EntityPool
	{
	private:
		std::vector<T*> m_FreeObjects;
		alignas(T) char m_Pool[PoolSize][sizeof(T)];

	public:
		EntityPool()
		{
			for (int32 i = 0; i < PoolSize; i++)
				m_FreeObjects.push_back(reinterpret_cast< T* >(&m_Pool[i]));
		}

		EntityPool(const EntityPool& pool) = delete;
		~EntityPool() = default;

		T* Allocate()
		{
			//TODO: add assert when all objects are used.
			T* obj = m_FreeObjects.back(); m_FreeObjects.pop_back();
			return new (obj) T();
		}

		void Destroy(T* obj)
		{
			obj->~T();
			m_FreeObjects.push_back(obj);
		}
	};
}