#pragma once

#define REGISTER_RESOURCE(aResource) REGISTER_RESOURCE_SYSTEM(aResource) REGISTER_ASSET_DATABASE(aResource) REGISTER_ASSET_MANAGER(aResource)

class Resource
{
public:
	Resource() noexcept = default;
	virtual ~Resource() noexcept = default;
	inline virtual bool ShouldHotReload() const { return true; }

	virtual void Inspect() {};
};
