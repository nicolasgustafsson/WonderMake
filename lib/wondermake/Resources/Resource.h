#pragma once

#define WM_REGISTER_RESOURCE(aResource) WM_REGISTER_RESOURCE_SYSTEM(aResource) WM_REGISTER_ASSET_DATABASE(aResource)

class Resource
{
public:
	Resource() noexcept = default;
	virtual ~Resource() noexcept = default;
	inline virtual bool ShouldHotReload() const { return true; }

	virtual void Inspect() {};
};
