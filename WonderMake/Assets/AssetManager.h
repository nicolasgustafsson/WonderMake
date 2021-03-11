#pragma once
template <typename TAsset>
class AssetManager : public System<>
{
	std::optional<AssetRef<TAsset>> GetAsset(const std::filesystem::path& aPath)
	{
		if (!std::filesystem::exists(aPath))
			return std::nullopt;

		//[Nicos]: hash_value will resolve in same value even in different formatting, I think
		const size_t hashedPath = std::filesystem::hash_value(aPath);
		
		if (auto&& asset = myAssets.SafeGet(hashedPath))
			return AssetRef(asset);

		return AssetRef(myAssets.Add(hashedPath, std::move(TAsset(aPath))));
	}

	Container<TAsset, Key<size_t>> myAssets;
};