#pragma once
#include "Resources/ResourceSystem.h"
#include "Utilities/Id.h"

#include <filesystem>

struct SAssetMetadata
{
	std::filesystem::path Filepath;
	Id AssetId;
};

template <typename TAssetType>
class Asset
{
public:
	Asset(std::filesystem::path aPath, Id aAssetId)
	{
		myMetadata.Filepath = std::move(aPath);
		myMetadata.AssetId = aAssetId;
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
		if (!myResource)
			ImGui::Text(myMetadata.Filepath.string().c_str());
		else if (ImGui::TreeNode(myMetadata.Filepath.string().c_str()))
		{
			ImGui::TreePop();
		}
	}
	
	ResourceProxy<TAssetType> LoadAsset(ResourceSystem<TAssetType>& aResourceSystem)
	{
		myResource.emplace(aResourceSystem.GetResource(myMetadata.Filepath));

		return *myResource;
	}

	std::optional<ResourceProxy<TAssetType>> myResource;
	SAssetMetadata myMetadata;
};