#pragma once
#include <filesystem>

struct SAssetMetadata
{
	std::filesystem::path Filepath;
};

template <typename TAssetType>
class Asset
{
public:
	Asset(std::filesystem::path aPath)
	{
		myMetadata.Filepath = std::move(aPath);
	}

	void Inspect()
	{
		ImGui::Text(myMetadata.Filepath.string().c_str());
	}

	ResourceProxy<TAssetType> Get()
	{
		if (myResource)
			return myResource;

		return LoadAsset();
	}
	
	ResourceProxy<TAssetType> LoadAsset()
	{
		myResource.emplace(ResourceSystem<TAssetType>()->GetResource(myMetadata.Filepath));

		return myResource;
	}

	std::optional<ResourceProxy<TAssetType>> myResource;
	SAssetMetadata myMetadata;
};