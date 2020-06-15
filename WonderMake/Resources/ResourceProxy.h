#pragma once

#include <cassert>
#include <memory>
#include <mutex>

template<typename TResource>
struct SResource
{
	std::mutex myLock;
	std::shared_ptr<TResource> myPointer = nullptr;
	i32 myGeneration = 0;
};

enum EResourceGenerationResult
{
	UpToDate,
	NewGeneration
};

template<typename TResource>
class ResourceProxy final
{
public:
	ResourceProxy() = default;
	inline ResourceProxy(std::shared_ptr<SResource<TResource>>&& aResource)
		: myResource(aResource)
	{}

	inline bool IsValid() const;

	inline TResource* operator->() const;
	inline TResource& operator*() const;

	inline operator bool() const;

	EResourceGenerationResult CheckGeneration();

private:
	inline void Validate() const;

	std::shared_ptr<SResource<TResource>> myResource = nullptr;

	//Nicos todo: make this mutable
	i32 myGeneration = 0;
};

template<typename TResource>
EResourceGenerationResult ResourceProxy<TResource>::CheckGeneration()
{
	std::lock_guard<decltype(myResource->myLock)> lock(myResource->myLock);

	if (myGeneration != myResource->myGeneration)
	{
		myGeneration = myResource->myGeneration;
		return EResourceGenerationResult::NewGeneration;
	}

	return EResourceGenerationResult::UpToDate;
}

template<typename TResource>
inline bool ResourceProxy<TResource>::IsValid() const
{
	if (myResource == nullptr)
	{
		return false;
	}
	std::lock_guard<decltype(myResource->myLock)> lock(myResource->myLock);
	return myResource->myPointer != nullptr;
}

template<typename TResource>
inline TResource* ResourceProxy<TResource>::operator->() const
{
	std::lock_guard<decltype(myResource->myLock)> lock(myResource->myLock);
	Validate();
	return myResource->myPointer.get();
}

template<typename TResource>
inline TResource& ResourceProxy<TResource>::operator*() const
{
	std::lock_guard<decltype(myResource->myLock)> lock(myResource->myLock);
	Validate();
	return *myResource->myPointer;
}

template<typename TResource>
inline ResourceProxy<TResource>::operator bool() const
{
	return IsValid();
}

template<typename TResource>
inline void ResourceProxy<TResource>::Validate() const
{
	if (myResource == nullptr
		|| myResource->myPointer == nullptr)
	{
		WmLog(TagError, "Resource proxy validation failed.");
		assert(false);
	}
}
