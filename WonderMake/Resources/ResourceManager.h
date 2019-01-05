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
	std::unordered_map<std::string, std::weak_ptr<SResource<TResource>>> myResources;
};

template<typename TResource>
ResourceProxy<TResource> ResourceManager<TResource>::GetResource(const std::filesystem::path& aPath)
{
	std::lock_guard<decltype(myLock)> lock(myLock);
	auto it = myResources.find(aPath.string());
	if (it == myResources.end())
	{
		auto rawResource = new SResource<TResource>();
		rawResource->myPointer = std::make_shared<TResource>(aPath);
		std::shared_ptr<SResource<TResource>> resource(rawResource, [this, aPath](SResource<TResource>* aResource)
		{
			std::lock_guard<decltype(myLock)> lock(myLock);
			auto it = myResources.find(aPath.string());
			if (it == myResources.end())
			{
				WmLog(TagError, '[', typeid(TResource).name(), "]Unknown resource deletion requested: ", aResource);
				return;
			}
			myResources.erase(it);
			delete aResource;
		});
		myResources[aPath.string()] = resource;
		return ResourceProxy<TResource>(std::move(resource));
	}
	return ResourceProxy<TResource>(std::shared_ptr<SResource<TResource>>(it->second));
}
