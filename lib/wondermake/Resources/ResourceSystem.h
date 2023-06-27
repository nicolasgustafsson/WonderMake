#pragma once

#include "Resources/ResourceProxy.h"

#include "wondermake-base/Logger.h"
#include "wondermake-base/System.h"
#include "wondermake-base/SystemGlobal.h"
#include "wondermake-base/WmLogTags.h"

#include "wondermake-utility/plf_colony.h"

#include <filesystem>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>

template<typename TResource>
class ResourceSystem
	: public System<
		Policy::Set<>,
		STrait::Set<
			STGui,
			STSingleton,
			STWonderMake>>
{
public:
	ResourceSystem() = default;
	ResourceProxy<TResource> GetResource(const std::filesystem::path& aPath);
	plf::colony<ResourceProxy<TResource>> GetAllResources() const;

protected:
	inline void ResourceDeleter(std::filesystem::path aPath, SResource<TResource>* const aResource);

	std::mutex myLock;
	std::unordered_map<std::string, std::weak_ptr<SResource<TResource>>> myResources;
};

#define WM_REGISTER_RESOURCE_SYSTEM(aResource) _WM_REGISTER_SYSTEM_IMPL(ResourceSystem<aResource>, aResource)

template<typename TResource>
plf::colony<ResourceProxy<TResource>> ResourceSystem<TResource>::GetAllResources() const
{
	plf::colony<ResourceProxy<TResource>> returnVal;
	for (auto&& resource : myResources)
	{
		returnVal.emplace(resource.second.lock());
	}
	return returnVal;
}

template<typename TResource>
ResourceProxy<TResource> ResourceSystem<TResource>::GetResource(const std::filesystem::path& aPath)
{
	SResource<TResource>* rawResource = nullptr;
	std::shared_ptr<SResource<TResource>> resource;

	{
		std::lock_guard<decltype(myLock)> lock(myLock);
	
		auto it = myResources.find(aPath.string());
		if (it != myResources.end())
		{
			return ResourceProxy<TResource>(it->second.lock());
		}

		rawResource = new SResource<TResource>();
		resource = std::shared_ptr<SResource<TResource>>(rawResource, [&, aPath](auto aResource) { ResourceDeleter(aPath, aResource); });
		myResources[aPath.string()] = resource;
	}

	std::lock_guard<decltype(rawResource->myLock)> lock(rawResource->myLock);
	rawResource->myPointer = std::make_shared<TResource>(aPath);

	return ResourceProxy<TResource>(std::move(resource));
}

template<typename TResource>
inline void ResourceSystem<TResource>::ResourceDeleter(std::filesystem::path aPath, SResource<TResource>* const aResource)
{
	std::lock_guard<decltype(myLock)> lock(myLock);

	const auto resourceIt = myResources.find(aPath.string());
	if (resourceIt == myResources.cend())
	{
		WmLogError(TagWonderMake << TagWmResources << '[' << typeid(TResource).name() << "]Unknown resource deletion requested: " << aResource << '.');
		return;
	}
	myResources.erase(resourceIt);

	GetExecutor().Execute([aResource]
		{
			delete aResource;
		});
}
