#pragma once
#include "Common.hpp"

namespace Hyper
{
	class HYPER_API Guid
	{
	public:
		union
		{
			struct
			{
				uint32 A;
				uint32 B;
				uint32 C;
				uint32 D;
			};

			char Raw[16];
			uint32 Values[4];
		};

		bool operator==(const Guid& guid) const
		{
			return
				A == guid.A &&
				B == guid.B &&
				C == guid.C &&
				D == guid.D;
		};

		static Guid Random();
		String ToString() const;
	};
}

template<>
struct HYPER_API std::hash<Hyper::Guid>
{
	std::size_t operator()(const Hyper::Guid& k) const noexcept
	{
		uint32 hash = 2166136261u;
		hash = (hash ^ k.A) * 16777619u;
		hash = (hash ^ k.B) * 16777619u;
		hash = (hash ^ k.C) * 16777619u;
		hash = (hash ^ k.D) * 16777619u;
		return hash;
	}
};