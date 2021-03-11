#pragma once
#include <optional>
#include "Utilities/Id.h"
#include <json/json.hpp>
#include "Utilities/Json/JsonExtensions.h"
#include "Asset.h"

template<class TAssetType>
struct SAssetLink
{
	AssetOld<TAssetType>* CachedAsset;
	std::optional<Id> AssetId;
	std::optional<Id> SavedAssetId;
	bool IsSelectingAsset = false;
};

//nicos: TODO: make sure this is used and that we update assets when we change the links
template<class T>
struct SAssetLinkProxy
{
	SAssetLink<T>& AssetLink;
};

template<class T>
inline void to_json(json& aJson, const SAssetLink<T>& aAssetLink)
{
	aJson = { {"AssetId", aAssetLink.SavedAssetId} };
}

template<class T>
inline void from_json(const json& aJson, SAssetLink<T>& aAssetLink)
{
	aAssetLink.AssetId = aJson["AssetId"].get<std::optional<Id>>();
	aAssetLink.SavedAssetId = aAssetLink.AssetId;
}

