#pragma once
#include "Resources/CreateResource.h"
#include "Resources/Resource.h"
#include "Resources/ResourceProxy.h"

#include "Utilities/Singleton.h"

#include <filesystem>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>

template<typename TResource>
class ResourceManager
	: public Singleton<ResourceManager<TResource>>
{
public:
	ResourceProxy<TResource> GetResource(const std::filesystem::path& aPath);

	template<typename TJob>
	inline void SetCreateResourceJob();

protected:
	friend class Singleton<ResourceManager<TResource>>;

	inline void ResourceDeleter(std::filesystem::path aPath, SResource<TResource>* const aResource);

	inline void OnCreateResourceComplete(const std::filesystem::path aPath);

	std::function<void(const std::filesystem::path&)> myStartCreateJob;

	std::mutex myLock;
	std::unordered_map<std::string, std::weak_ptr<SResource<TResource>>> myResources;
	std::unordered_map<std::string, std::shared_ptr<CreateResource<TResource>>> myCreateResourceJobs;
};

template<typename TResource>
ResourceProxy<TResource> ResourceManager<TResource>::GetResource(const std::filesystem::path& aPath)
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
		resource = std::shared_ptr<SResource<TResource>>(rawResource, std::bind(&ResourceManager<TResource>::ResourceDeleter, this, aPath, std::placeholders::_1));
		myResources[aPath.string()] = resource;
	}

	if (myStartCreateJob)
	{
		myStartCreateJob(aPath);
	}
	else
	{
		std::lock_guard<decltype(rawResource->myLock)> lock(rawResource->myLock);
		rawResource->myPointer = std::make_shared<TResource>(aPath);
	}

	return ResourceProxy<TResource>(std::move(resource));
}

template<typename TResource>
template<typename TJob>
inline void ResourceManager<TResource>::SetCreateResourceJob()
{
	std::lock_guard<decltype(myLock)> lock(myLock);
	myStartCreateJob = [&](const std::filesystem::path& aPath)
	{
		auto job = std::make_shared<TJob>();
		job->Setup(aPath, std::bind(&ResourceManager<TResource>::OnCreateResourceComplete, this, aPath));
		myCreateResourceJobs[aPath.string()] = job;
		job->Start();
	};
}

template<typename TResource>
inline void ResourceManager<TResource>::ResourceDeleter(std::filesystem::path aPath, SResource<TResource>* const aResource)
{
	std::lock_guard<decltype(myLock)> lock(myLock);

	const auto resourceIt = myResources.find(aPath.string());
	if (resourceIt == myResources.cend())
	{
		WmLog(TagError, '[', typeid(TResource).name(), "]Unknown resource deletion requested: ", aResource);
		return;
	}
	myResources.erase(resourceIt);

	const auto jobIt = myCreateResourceJobs.find(aPath.string());
	if (jobIt != myCreateResourceJobs.cend())
	{
		myCreateResourceJobs.erase(jobIt);
	}

	WmDispatchTask([aResource] { delete aResource; }, aResource->myPointer->GetOwnerRoutineId());
}

template<typename TResource>
inline void ResourceManager<TResource>::OnCreateResourceComplete(const std::filesystem::path aPath)
{
	std::shared_ptr<SResource<TResource>> resourceStruct;
	std::shared_ptr<CreateResource<TResource>> resourceJob;

	{
		std::lock_guard<decltype(myLock)> lock(myLock);

		auto jobIt = myCreateResourceJobs.find(aPath.string());
		if (jobIt == myCreateResourceJobs.cend())
		{
			return;
		}
		resourceJob = std::move(jobIt->second);
		myCreateResourceJobs.erase(jobIt);

		const auto resourceIt = myResources.find(aPath.string());
		if (resourceIt != myResources.cend())
		{
			return;
		}
		resourceStruct = resourceIt->second.lock();
	}

	std::lock_guard<decltype(resourceStruct->myLock)> lock(resourceStruct->myLock);
	resourceStruct->myPointer = std::move(resourceJob->myResource);
}