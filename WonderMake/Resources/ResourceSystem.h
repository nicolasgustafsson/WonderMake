#pragma once
#include "Resources/CreateResource.h"
#include "Resources/Resource.h"
#include "Resources/ResourceProxy.h"

#include "System/System.h"

#include <filesystem>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>
#include "Message/MessageTypes.h"
#include "Message/MessageSubscriber.h"

template<typename TResource>
class ResourceSystem
	: public System<>
{
public:
	ResourceSystem(Dependencies&& aDependencies);
	ResourceProxy<TResource> GetResource(const std::filesystem::path& aPath);

	template<typename TJob>
	inline void SetCreateResourceJob();

protected:
	inline void ResourceDeleter(std::filesystem::path aPath, SResource<TResource>* const aResource);

	inline void OnCreateResourceComplete(const std::filesystem::path aPath);

	inline void OnFileChange(const SFileChangedMessage& aFileChangedMessage);

	std::function<void(const std::filesystem::path&)> myStartCreateJob;

	MessageSubscriber mySubscriber;
	std::mutex myLock;
	std::unordered_map<std::string, std::weak_ptr<SResource<TResource>>> myResources;
	std::unordered_map<std::string, std::shared_ptr<CreateResource<TResource>>> myCreateResourceJobs;
};

#define REGISTER_RESOURCE_SYSTEM(aResource) _REGISTER_SYSTEM_IMPL(ResourceSystem<aResource>, aResource)

template<typename TResource>
void ResourceSystem<TResource>::OnFileChange(const SFileChangedMessage& aFileChangedMessage)
{
	for (auto&& str : myResources)
	{
		std::filesystem::path strPath(str.first);
		
		//[Nicos]: The file may have been destroyed(if it is a temporary) which makes equivalent crash, so check it in the hot loop
		//[Nicos]: TODO: Solve this properly; we should not send file changed messages when a file has been created.
		if (!std::filesystem::exists(aFileChangedMessage.FilePath))
			return;

		if (std::filesystem::equivalent(strPath, aFileChangedMessage.FilePath))
		{
			std::weak_ptr<SResource<TResource>> weakResource = myResources[str.first];

			if (std::shared_ptr<SResource<TResource>> strongResource = weakResource.lock())
			{
				if (myStartCreateJob)
				{
					myStartCreateJob(strPath);
				}
				else
				{
					std::lock_guard<decltype(strongResource->myLock)> lock(strongResource->myLock);
					strongResource->myPointer = std::make_shared<TResource>(aFileChangedMessage.FilePath);
				}

				strongResource->myGeneration++;

				WmLog(TagSuccess, "Reloaded asset: ", aFileChangedMessage.FilePath.string());
			}
		}
	}
}

template<typename TResource>
ResourceSystem<TResource>::ResourceSystem(Dependencies&& aDependencies)
	: Super(std::move(aDependencies))
	, mySubscriber(ERoutineId::Logic, BindHelper(&ResourceSystem<TResource>::OnFileChange, this))
{}

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
inline void ResourceSystem<TResource>::SetCreateResourceJob()
{
	std::lock_guard<decltype(myLock)> lock(myLock);
	myStartCreateJob = [&](const std::filesystem::path& aPath)
	{
		auto job = std::make_shared<TJob>();
		job->Setup(aPath, [&, aPath] { OnCreateResourceComplete(aPath); });
		myCreateResourceJobs[aPath.string()] = job;
		job->Start();
	};
}

template<typename TResource>
inline void ResourceSystem<TResource>::ResourceDeleter(std::filesystem::path aPath, SResource<TResource>* const aResource)
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
inline void ResourceSystem<TResource>::OnCreateResourceComplete(const std::filesystem::path aPath)
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