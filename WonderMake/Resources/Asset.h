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

	bool operator==(const Asset<TAssetType>& aOther) const
	{
		return aOther.myMetadata.Filepath == myMetadata.Filepath;
	}

	bool operator!=(const Asset<TAssetType>& aOther) const
	{
		return !(aOther == *this);
	}

	bool Exists() const 
	{
		return std::filesystem::exists(myMetadata.Filepath);
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