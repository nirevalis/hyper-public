#pragma once

#include <Core/Common.hpp>
#include "Entities/PlayerEntity.hpp"
#include <Scene/Scene.hpp>

namespace Hyper
{
	class HYPER_API LocalPlayer
	{
	public:
		PlayerEntity* Player;
		Scene* Scene = nullptr;

		LocalPlayer();
		~LocalPlayer() = default;
	};
}