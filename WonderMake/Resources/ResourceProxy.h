#pragma once

#include "Constants.h"

#include "WonderMakeBase/Logger.h"

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

	EResourceGenerationResult UpdateGeneration() requires Constants::EnableAssetHotReload;
	constexpr EResourceGenerationResult UpdateGeneration() const requires (!Constants::EnableAssetHotReload);

private:
	inline void Validate() const;

	std::shared_ptr<SResource<TResource>> myResource = nullptr;

	i32 myGeneration = 0;
};

template<typename TResource>
EResourceGenerationResult ResourceProxy<TResource>::UpdateGeneration() requires Constants::EnableAssetHotReload
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
constexpr EResourceGenerationResult ResourceProxy<TResource>::UpdateGeneration() const requires (!Constants::EnableAssetHotReload)
{
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
		WM_LOG_ERROR("Resource proxy validation failed.");
		assert(false);
	}
}
