#pragma once

#include "Resources/ResourceSystem.h"

#define WM_REGISTER_RESOURCE(aResource) WM_REGISTER_RESOURCE_SYSTEM(aResource)

class Resource
{
public:
	Resource() noexcept = default;
	virtual ~Resource() noexcept = default;
	inline virtual bool ShouldHotReload() const { return true; }

	virtual void Inspect() {};
};

template<typename TResource>
inline [[nodiscard]] std::string GetResourceTypeName()
{
	std::string resTypeName = typeid(TResource).name();

	if (resTypeName.starts_with("class "))
		resTypeName.erase(resTypeName.begin(), resTypeName.begin() + 6);

	if (resTypeName.starts_with("struct "))
		resTypeName.erase(resTypeName.begin(), resTypeName.begin() + 7);

	return resTypeName;
}
