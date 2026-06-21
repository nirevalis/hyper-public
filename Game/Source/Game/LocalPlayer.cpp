#include "LocalPlayer.hpp"
#include <Engine/GameEngine.hpp>
#include <Core/Serialization/Stream.hpp>

namespace Hyper
{
	LocalPlayer::LocalPlayer()
	{
		Scene = new class Scene();

		GameEngine& engine = GameEngine::Get();


		Player = new PlayerEntity();
		Player->LocalPlayer = this;
		Scene->Register(Player);
	}
}