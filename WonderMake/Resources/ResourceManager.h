#pragma once
#include "Resources/ResourceProxy.h"

#include "Utilities/Singleton.h"

#include <filesystem>
#include <memory>
#include <mutex>
#include <unordered_map>

template<typename TResource>
class ResourceManager : public Singleton<ResourceManager<TResource>>
{
public:
	ResourceProxy<TResource> GetResource(const std::filesystem::path& aPath);

protected:
	friend class Singleton<ResourceManager<TResource>>;

	//can't hash paths :(
	std::mutex myLock;
	std::unordered_map<std::string, std::weak_ptr<TResource>> myResources;
};

template<typename TResource>
ResourceProxy<TResource> ResourceManager<TResource>::GetResource(const std::filesystem::path& aPath)
{
	std::lock_guard<decltype(myLock)> lock(myLock);
	auto it = myResources.find(aPath.string());
	if (it == myResources.end())
	{
		TResource* rawResource = new TResource(aPath);
		std::shared_ptr<TResource> resource(rawResource, [this, aPath](TResource* aResource)
		{
			std::lock_guard<decltype(myLock)> lock(myLock);
			auto it = myResources.find(aPath.string());
			if (it == myResources.end())
			{
				WmLog(TagError, '[', typeid(TResource).name(), "] Unknown resource deletion requested: ", aResource);
				return;
			}
			myResources.erase(it);
			delete aResource;
		});
		myResources[aPath.string()] = resource;
		return ResourceProxy<TResource>(std::move(resource));
	}
	return ResourceProxy<TResource>(std::shared_ptr<TResource>(it->second));
}
