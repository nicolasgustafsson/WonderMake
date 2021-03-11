#pragma once

template<typename TAsset>
class AssetRef
{
public:
	AssetRef(TAsset& aAsset)
		: myAsset(aAsset)
	{
	}

	TAsset* operator->() const
	{
		return &myAsset;
	}

	TAsset& operator*() const
	{
		return myAsset;
	}
private:

	TAsset& myAsset;
};

