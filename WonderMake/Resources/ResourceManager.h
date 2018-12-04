#pragma once
#include <unordered_map>
#include <filesystem>
#include <memory>
#include "../Utilities/Singleton.h"

template<typename T>
class ResourceManager : public Singleton<ResourceManager<T>>
{
public:
	T* GetResource(const std::filesystem::path& Path);

protected:
	friend class Singleton<ResourceManager<T>>;

	//can't hash paths :(
	std::unordered_map<std::string, std::unique_ptr<T>> LoadedResources;
};

template<typename T>
T* ResourceManager<T>::GetResource(const std::filesystem::path& Path)
{
	auto ret = LoadedResources.try_emplace(Path.string(), std::make_unique<T>(Path));
	return ret.first->second.get();
}
