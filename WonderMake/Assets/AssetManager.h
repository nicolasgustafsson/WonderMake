#pragma once
#include "AssetRef.h"
#include <optional>
#include "Utilities/StaticRegistry.h"

template <typename TAsset>
class AssetManager : public System<>
{
public:
	std::optional<AssetRef<TAsset>> GetAsset(const std::filesystem::path& aPath)
	{
		static bool first = true;
		if (first)
		{
			for (auto&& entry : GetRegistry().entries)
			{
				WmLog(entry);
			}
			first = false;
		}

		if (!std::filesystem::exists(aPath))
			return std::nullopt;

		//[Nicos]: hash_value will resolve in same value even in different formatting, I think
		const size_t hashedPath = std::filesystem::hash_value(aPath);
		
		if (auto&& asset = myAssets.SafeGet(hashedPath))
			return std::optional(AssetRef(*asset));
		
		return std::optional(AssetRef(myAssets.Emplace(hashedPath, aPath)));
	}

private:
	Container<TAsset, Key<size_t>> myAssets;
};

#define REGISTER_ASSET_MANAGER(aAsset) _REGISTER_SYSTEM_IMPL(AssetManager<aAsset>, aAsset##AssetManager)