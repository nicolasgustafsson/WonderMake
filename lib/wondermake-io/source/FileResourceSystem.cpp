#include "wondermake-io/FileResourceSystem.h"

#include "wondermake-base/SystemGlobal.h"

WM_REGISTER_SYSTEM(FileResourceSystem);

void FileResourceSystem::KeepLoaded(std::string_view aResourceTypeName, const FilePath& aFilePath)
{
	auto itType = myResourceTypes.find(aResourceTypeName);

	if (itType == myResourceTypes.end())
	{
		auto [promise, future] = MakeAsync<const std::type_index>();

		auto [it, _] = myResourceTypes.insert(std::make_pair(aResourceTypeName, std::move(future)));
		myResourceTypeSetFuncs.insert(std::make_pair(aResourceTypeName, std::move(promise)));

		itType = it;
	}

	auto& future = itType->second;

	future.ThenRun(GetExecutor(), FutureRunResult([this, aFilePath](const auto& aTypeId) mutable
		{
			auto itFactory = myFactories.find(aTypeId);

			if (itFactory == myFactories.end())
			{
				auto [promise, future] = MakeAsync<const SCreateFuncs>();

				auto [it, _] = myFactories.insert(std::make_pair(aTypeId, std::move(future)));
				myFactorySetFuncs.insert(std::make_pair(aTypeId, std::move(promise)));

				itFactory = it;
			}

			auto& future = itFactory->second;

			future.ThenRun(GetExecutor(), FutureRunResult([this, aFilePath, aTypeId](const auto& aCreateFuncs) mutable
				{
					auto anyFuture = aCreateFuncs.GetAsAny(aFilePath);

					auto& fileMap = myKeepLoadedResources[aTypeId];

					fileMap.insert(std::make_pair(std::move(aFilePath), std::move(anyFuture)));
				}));
		}));

}

void FileResourceSystem::Unload(std::string_view aResourceTypeName, const FilePath& aFilePath)
{
	auto itType = myResourceTypes.find(aResourceTypeName);

	if (itType == myResourceTypes.end())
	{
		auto [promise, future] = MakeAsync<const std::type_index>();

		auto [it, _] = myResourceTypes.insert(std::make_pair(aResourceTypeName, std::move(future)));
		myResourceTypeSetFuncs.insert(std::make_pair(aResourceTypeName, std::move(promise)));

		itType = it;
	}

	auto& future = itType->second;

	future.ThenRun(GetExecutor(), FutureRunResult([this, aFilePath](const auto& aTypeId) mutable
		{
			auto& fileMap = myKeepLoadedResources[aTypeId];

			auto it = fileMap.find(aFilePath);

			if (it == fileMap.end())
				return;

			auto anyFuture = std::move(it->second);

			fileMap.erase(it);

			anyFuture.reset();

		}));
}
