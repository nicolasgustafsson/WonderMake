#pragma once

#include "wondermake-io/FileResource.h"
#include "wondermake-io/FileResourceProxy.h"
#include "wondermake-io/FileWatcherSystem.h"

#include "wondermake-base/Logger.h"
#include "wondermake-base/System.h"
#include "wondermake-base/WmLogTags.h"

#include "wondermake-utility/EventSubscriber.h"
#include "wondermake-utility/FilePath.h"
#include "wondermake-utility/Future.h"
#include "wondermake-utility/SharedReference.h"
#include "wondermake-utility/UniqueFunction.h"

#include <any>
#include <functional>
#include <typeindex>
#include <unordered_map>

class FileResourceSystem
	: public System<
		Policy::Set<
			PAdd<FileWatcherSystem, PWrite>>,
		STrait::Set<
			STSingleton,
			STWonderMake>>
{
public:
	template<CFileResource TResource>
	void SetFactory(auto aCreateCallable, auto aCreateWithParamsCallable)
	{
		auto getAsAny = [this](const FilePath& aFilePath) -> std::any
		{
			return GetResource<TResource>(aFilePath);
		};
		auto itSet = myFactorySetFuncs.find(typeid(TResource));

		if (itSet == myFactorySetFuncs.end())
		{
			auto itFac = myFactories.find(typeid(TResource));

			if (itFac != myFactories.end())
			{
				WmLogError(TagWonderMake << TagWmResources << "Resource factory set twice; resource type: " << GetFileResourceTypeName<TResource>() << '.');

				return;
			}

			auto future = MakeCompletedFuture<const SCreateFuncs>(
				SCreateFuncs
				{
					.Create				= std::move(aCreateCallable),
					.CreateWithParams	= std::move(aCreateWithParamsCallable),
					.GetAsAny			= std::move(getAsAny)
				});

			myFactories.emplace(std::make_pair<std::type_index>(typeid(TResource), std::move(future)));

			SetTypeName<TResource>();

			return;
		}

		auto promise = std::move(itSet->second);

		myFactorySetFuncs.erase(itSet);

		promise.Complete(
			SCreateFuncs
			{
				.Create				= std::move(aCreateCallable),
				.CreateWithParams	= std::move(aCreateWithParamsCallable),
				.GetAsAny			= std::move(getAsAny)
			});

		SetTypeName<TResource>();
	}

	template<CFileResource TResource>
	Future<FileResourceRef<TResource>> GetResource(const FilePath& aPath)
	{
		auto future = GetResourceDataFromMap<TResource>(aPath);

		if (!future.IsCanceled())
			return future.ThenApply(InlineExecutor(), FutureApplyResult([](auto aDataPtr)
				{
					if (!aDataPtr)
						return MakeCanceledFuture<FileResourceRef<TResource>>();

					auto lock = aDataPtr->WeakPtr.lock();

					if (!lock)
						return MakeCanceledFuture<FileResourceRef<TResource>>();

					return MakeCompletedFuture<FileResourceRef<TResource>>(FileResourcePtr<TResource>(lock).ToRef());
				}));

		return CreateResourceFromFactoryMap<TResource>(aPath);
	}

	template<CFileResource TResource>
	void KeepLoaded(const FilePath& aFilePath)
	{
		std::type_index id = typeid(TResource);

		auto itFactory = myFactories.find(id);

		if (itFactory == myFactories.end())
		{
			auto [promise, future] = MakeAsync<const SCreateFuncs>();

			auto [it, _] = myFactories.insert(std::make_pair(id, std::move(future)));
			myFactorySetFuncs.insert(std::make_pair(id, std::move(promise)));

			itFactory = it;
		}

		auto& future = itFactory->second;

		future.ThenRun(GetExecutor(), FutureRunResult([this, aFilePath, id](const auto& aCreateFuncs) mutable
			{
				auto anyFuture = aCreateFuncs.GetAsAny(aFilePath);

				auto& fileMap = myKeepLoadedResources[id];

				fileMap.insert(std::make_pair(std::move(aFilePath), std::move(anyFuture)));
			}));
	}
	template<CFileResource TResource>
	void Unload(const FilePath& aFilePath)
	{
		std::type_index id = typeid(TResource);

		auto& fileMap = myKeepLoadedResources[id];

		auto it = fileMap.find(aFilePath);

		if (it == fileMap.end())
			return;

		auto anyFuture = std::move(it->second);

		fileMap.erase(it);

		anyFuture.reset();
	}
	
	void KeepLoaded(std::string_view aResourceTypeName, const FilePath& aFilePath);
	void Unload(std::string_view aResourceTypeName, const FilePath& aFilePath);

private:
	struct SResourceDataBase {};
	template<CFileResource TResource>
	struct SResourceData
		: public SResourceDataBase
	{
		std::weak_ptr<SharedReference<TResource>>	WeakPtr;
		std::vector<EventSubscriber>				OnFileChanges;
	};
	struct SCreateFuncs
	{
		using CreateType		= std::function<Future<SharedReference<FileResourceBase>>(FilePath)>;
		using CreateWithIdType	= std::function<Future<SharedReference<FileResourceBase>>(FilePath, FileResourceId, u32)>;
		using GetAsAnyType		= std::function<std::any(FilePath)>;

		CreateType			Create;
		CreateWithIdType	CreateWithParams;
		GetAsAnyType		GetAsAny;
	};

	using ResourceFileMap		= std::unordered_map<FilePath, Future<const std::shared_ptr<SResourceDataBase>>>;
	using AnyFileMap			= std::unordered_map<FilePath, std::any>;

	using ResourceTypeMap		= std::unordered_map<std::string_view, Future<const std::type_index>>;
	using ResourceTypeSetMap	= std::unordered_map<std::string_view, Promise<const std::type_index>>;
	using FactoryMap			= std::unordered_map<std::type_index, Future<const SCreateFuncs>>;
	using FactorySetMap			= std::unordered_map<std::type_index, Promise<const SCreateFuncs>>;
	using ResourceMap			= std::unordered_map<std::type_index, ResourceFileMap>;
	using AnyMap				= std::unordered_map<std::type_index, AnyFileMap>;

	template<CFileResource TResource>
	Future<SResourceData<TResource>*> GetResourceDataFromMap(const FilePath& aPath)
	{
		auto itType = myResources.find(typeid(TResource));

		if (itType == myResources.end())
			return MakeCanceledFuture<SResourceData<TResource>*>();

		auto& resourceMap = itType->second;

		auto itFile = resourceMap.find(aPath);

		if (itFile == resourceMap.end())
			return MakeCanceledFuture<SResourceData<TResource>*>();

		auto& future = itFile->second;

		return future.ThenApply(InlineExecutor(), FutureApplyResult([](const auto& aDataPtr)
			{
				if (!aDataPtr)
					return MakeCanceledFuture<SResourceData<TResource>*>();

				auto castedDataPtr = static_cast<SResourceData<TResource>*>(aDataPtr.get());

				return MakeCompletedFuture<SResourceData<TResource>*>(castedDataPtr);
			}));
	}

	template<CFileResource TResource>
	Future<FileResourceRef<TResource>> CreateResourceFromFactoryMap(const FilePath& aPath)
	{
		auto itFactory = myFactories.find(typeid(TResource));

		if (itFactory == myFactories.end())
		{
			auto [promise, future] = MakeAsync<const SCreateFuncs>();

			auto [it, _] = myFactories.insert(std::make_pair(std::type_index(typeid(TResource)), std::move(future)));
			myFactorySetFuncs.insert(std::make_pair(std::type_index(typeid(TResource)), std::move(promise)));

			itFactory = it;
		}
		
		auto& future = itFactory->second;

		return future.ThenApply(GetExecutor(), FutureApplyResult([this, aPath](const auto& aCreateFuncs)
			{
				auto futureRes = aCreateFuncs.Create(aPath)
					.ThenApply(GetExecutor(), FutureApplyResult([this, aPath](SharedReference<FileResourceBase> aReference) mutable
						{
							auto castedRef = StaticReferenceCast<TResource>(std::move(aReference));

							auto ptr = std::shared_ptr<SharedReference<TResource>>(new SharedReference<TResource>(std::move(castedRef)), [this, path = std::move(aPath)](SharedReference<TResource>* aPtr) mutable
								{
									GetExecutor().Execute([this, path = std::move(path), aPtr]()
										{
											RemoveResource<TResource>(path);

											delete aPtr;
										});
								});

							auto dRef = SharedReference<SharedReference<TResource>>::FromPointer(std::move(ptr)).Unwrap();

							return MakeCompletedFuture<const SharedReference<SharedReference<TResource>>>(std::move(dRef));
						}));
		
				myResources[typeid(TResource)][aPath] = futureRes
					.ThenApply(GetExecutor(), FutureApplyResult([this](const auto& aResourceRef)
						{
							SResourceData<TResource> data =
							{
								.WeakPtr		= aResourceRef.Weak(),
								.OnFileChanges	= OnFileResourceUpdate<TResource>(**aResourceRef)
							};

							auto dataPtr = std::make_shared<SResourceData<TResource>>(std::move(data));

							return MakeCompletedFuture<const std::shared_ptr<SResourceDataBase>>(std::move(dataPtr));
						}));

				return futureRes.ThenApply(InlineExecutor(), FutureApplyResult([](const auto& aResourceRef)
					{
						auto resPtr = FileResourcePtr<TResource>(aResourceRef);

						auto resRef = resPtr.ToRef();

						return MakeCompletedFuture<FileResourceRef<TResource>>(std::move(resRef));
					}));
			}));
		
	}

	template<CFileResource TResource>
	[[nodiscard]] std::vector<EventSubscriber> OnFileResourceUpdate(const TResource& aResource)
	{
		if constexpr (!TResource::IsReloadable)
			return EventSubscriber();
		else
		{
			std::vector<EventSubscriber> subscribers;
			const auto& resourcePath = aResource.Path();
			const auto& relatedFiles = aResource.RelatedFiles();

			subscribers.reserve(1 + relatedFiles.size());

			subscribers.emplace_back(
				Get<FileWatcherSystem>()
					.OnFileChange(resourcePath, GetExecutor(), Bind(&FileResourceSystem::UpdateFileResource<TResource>, this, resourcePath)));
			for (const auto& path : relatedFiles)
				subscribers.emplace_back(
					Get<FileWatcherSystem>()
					.OnFileChange(path, GetExecutor(), Bind(&FileResourceSystem::UpdateFileResource<TResource>, this, resourcePath)));

			return subscribers;
		}
			
	}
	template<CFileResource TResource>
	void UpdateFileResource(const FilePath& aResourcePath, const FilePath& /*aUpdatedFile*/)
	{
		auto future = GetResourceDataFromMap<TResource>(aResourcePath);

		future.ThenRun(GetExecutor(), FutureRunResult([this, aResourcePath](SResourceData<TResource>* aResData)
			{
				auto itFactory = myFactories.find(typeid(TResource));

				auto& future = itFactory->second;

				future.ThenRun(GetExecutor(), FutureRunResult([this, aResourcePath, aResData](const auto& aCreateFuncs)
					{
						if (!aResData)
						{
							WmLogError(TagWonderMake << TagWmResources << "Failed to update resource, unknown resource; path: " << aResourcePath << " resource type: " << GetFileResourceTypeName<TResource>() << '.');

							return;
						}

						auto ptr = aResData->WeakPtr.lock();

						if (!ptr)
						{
							RemoveResource<TResource>(aResourcePath);

							return;
						}

						auto id			= (*ptr)->Id();
						auto generation	= (*ptr)->Generation() + 1;

						WmLogInfo(TagWonderMake << TagWmResources << "Reloading resource; path: " << aResourcePath << " resource type: " << GetFileResourceTypeName<TResource>() << ", id: " << id << ", generation: " << generation << '.');

						aCreateFuncs.CreateWithParams(aResourcePath, id, generation)
							.ThenRun(GetExecutor(), FutureRunResult([this, aResourcePath, weakPtr = aResData->WeakPtr, id, generation](SharedReference<FileResourceBase> aReference)
								{
									auto ptr = weakPtr.lock();

									if (!ptr)
									{
										RemoveResource<TResource>(aResourcePath);

										return;
									}

									*ptr = StaticReferenceCast<TResource>(std::move(aReference));

									WmLogSuccess(TagWonderMake << TagWmResources << "Reloaded resource; id: " << id << ", generation: " << generation << '.');
								}))
							.Detach();
					}));
			}))
			.Detach();
	}

	template<CFileResource TResource>
	void RemoveResource(const FilePath& aPath)
	{
		auto itType = myResources.find(typeid(TResource));

		if (itType == myResources.end())
			return;

		auto& resourceMap = itType->second;

		auto itFile = resourceMap.find(aPath);

		if (itFile == resourceMap.end())
			return;

		resourceMap.erase(itFile);

		if (!resourceMap.empty())
			return;

		myResources.erase(itType);
	}

	template<CFileResource TResource>
	void SetTypeName()
	{
		std::string_view resourceName = GetFileResourceTypeName<TResource>();

		auto itType = myResourceTypes.find(resourceName);

		if (itType == myResourceTypes.end())
		{
			myResourceTypes.insert(std::make_pair(resourceName, MakeCompletedFuture<const std::type_index>(std::type_index(typeid(TResource)))));

			return;
		}

		auto itSet = myResourceTypeSetFuncs.find(resourceName);

		auto promise = std::move(itSet->second);

		myResourceTypeSetFuncs.erase(itSet);

		promise.Complete(std::type_index(typeid(TResource)));
	}

	ResourceTypeMap		myResourceTypes;
	ResourceTypeSetMap	myResourceTypeSetFuncs;

	FactoryMap			myFactories;
	FactorySetMap		myFactorySetFuncs;

	ResourceMap			myResources;
	AnyMap				myKeepLoadedResources;

};
