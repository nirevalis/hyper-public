#include "Skybox.hpp"

#include "Device.hpp"
#include "Asset/AssetRegistry.hpp"
#include "Assets/TextureAsset.hpp"
#include "Engine/GameEngine.hpp"

namespace Hyper
{
	Skybox::Skybox(const String& skyboxLocation)
	{
		DeviceHelper& helper = GameEngine::Get().GetDeviceHelper();

		TextureAsset* radiance = AssetRegistry::GetAs<TextureAsset>(FORMAT("{}/0", skyboxLocation));
		TextureAsset* irradiance = AssetRegistry::GetAs<TextureAsset>(FORMAT("{}/Irradiance/0", skyboxLocation));

		auto textureDesc = nvrhi::TextureDesc()
			.setDimension(nvrhi::TextureDimension::TextureCube)
			.setWidth(radiance->GetDescription().Width)
			.setHeight(radiance->GetDescription().Height)
			.setArraySize(6)
			.setFormat(nvrhi::Format::RGBA32_FLOAT)
			.setInitialState(nvrhi::ResourceStates::ShaderResource)
			.setKeepInitialState(true)
			.setDebugName(FORMAT("{} Radiance Map", skyboxLocation));

		m_Radiance = helper.GetDevice()->createTexture(textureDesc);

		textureDesc
			.setWidth(irradiance->GetDescription().Width)
			.setHeight(irradiance->GetDescription().Height)
			.setDebugName(FORMAT("{} Irradiance Map", skyboxLocation));

		m_Irradiance = helper.GetDevice()->createTexture(textureDesc);

		helper.Submit([this, skyboxLocation] (nvrhi::CommandListHandle cmd)
			{
				//Copy radiance textures

				for (uint32 i = 0; i < 6; i++)
				{
					nvrhi::TextureSlice slice = {};
					slice.arraySlice = i;
					cmd->copyTexture(
						m_Radiance,
						slice,
						AssetRegistry::GetAs<TextureAsset>(FORMAT("{}/{}", skyboxLocation, i))->GetTexture(),
						{}
					);
				}

				//Copy irradiance textures

				for (uint32 i = 0; i < 6; i++)
				{
					nvrhi::TextureSlice slice = {};
					slice.arraySlice = i;

					cmd->copyTexture(
						m_Radiance,
						slice,
						AssetRegistry::GetAs<TextureAsset>(FORMAT("{}/Irradiance/{}", skyboxLocation, i))->GetTexture(),
						{}
					);
				}
			});

	}

	nvrhi::TextureHandle Skybox::GetIrradiance()
	{
		return m_Irradiance;
	}

	nvrhi::TextureHandle Skybox::GetRadiance()
	{
		return m_Radiance;
	}
}
