#pragma once

#include <cassert>
#include <memory>

template<typename TResource>
class ResourceProxy final
{
public:
	ResourceProxy() = default;
	inline ResourceProxy(std::shared_ptr<TResource>&& aResource)
		: myResource(aResource)
	{}

	inline bool IsValid() const;

	inline TResource* operator->() const;
	inline TResource& operator*() const;

	inline operator bool() const;

private:
	inline void Validate() const;

	std::shared_ptr<TResource> myResource = nullptr;
};

template<typename TResource>
inline bool ResourceProxy<TResource>::IsValid() const
{
	return myResource != nullptr;
}

template<typename TResource>
inline TResource* ResourceProxy<TResource>::operator->() const
{
	Validate();
	return myResource.get();
}

template<typename TResource>
inline TResource& ResourceProxy<TResource>::operator*() const
{
	Validate();
	return *myResource;
}

template<typename TResource>
inline ResourceProxy<TResource>::operator bool() const
{
	return IsValid();
}

template<typename TResource>
inline void ResourceProxy<TResource>::Validate() const
{
	if (!IsValid())
	{
		WmLog(TagError, "Resource proxy validation failed.");
		assert(true);
	}
}
