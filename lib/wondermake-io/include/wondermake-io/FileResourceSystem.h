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
#include <variant>

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
		auto it = myFactories.find(typeid(TResource));

		if (it == myFactories.end())
		{
			myFactories.emplace(std::make_pair<std::type_index>(typeid(TResource), SCreateFuncs
				{
					.Create				= std::move(aCreateCallable),
					.CreateWithParams	= std::move(aCreateWithParamsCallable)
				}));

			return;
		}

		if (!std::holds_alternative<OnFactorySetList>(it->second))
		{
			WmLogError(TagWonderMake << TagWmResources << "Resource factory set twice; resource type: " << GetFileResourceTypeName<TResource>() << '.');

			return;
		}

		auto onSetList = std::move(std::get<OnFactorySetList>(it->second));

		it->second = SCreateFuncs
		{
			.Create				= std::move(aCreateCallable),
			.CreateWithParams	= std::move(aCreateWithParamsCallable)
		};

		for (auto& onSet : onSetList)
			std::move(onSet)();
	}
	
	template<CFileResource TResource>
	Future<FileResourceRef<TResource>> GetResource(const FilePath& aPath)
	{
		if (auto ptr = GetResourceFromMap<TResource>(aPath); ptr)
			return MakeCompletedFuture<FileResourceRef<TResource>>(FileResourcePtr<TResource>(ptr).ToRef());

		return CreateResourceFromFactoryMap<TResource>(aPath);
	}

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

		CreateType			Create;
		CreateWithIdType	CreateWithParams;
	};
	using OnFactorySetList		= std::vector<UniqueFunction<void()>>;

	using FactoryVariant		= std::variant<OnFactorySetList, SCreateFuncs>;
	using ResourceFileMap		= std::unordered_map<FilePath, std::unique_ptr<SResourceDataBase>>;

	using ResourceMap			= std::unordered_map<std::type_index, ResourceFileMap>;
	using FactoryMap			= std::unordered_map<std::type_index, FactoryVariant>;

	template<CFileResource TResource>
	SResourceData<TResource>* GetResourceDataFromMap(const FilePath& aPath)
	{
		auto itType = myResources.find(typeid(TResource));

		if (itType == myResources.end())
			return nullptr;

		const auto& resourceMap = itType->second;

		auto itFile = resourceMap.find(aPath);

		if (itFile == resourceMap.end())
			return nullptr;

		return static_cast<SResourceData<TResource>*>(itFile->second.get());
	}

	template<CFileResource TResource>
	std::shared_ptr<SharedReference<TResource>> GetResourceFromMap(const FilePath& aPath)
	{
		auto ptr = GetResourceDataFromMap<TResource>(aPath);

		if (!ptr)
			return nullptr;

		return ptr->WeakPtr.lock();
	}
	template<CFileResource TResource>
	Future<FileResourceRef<TResource>> CreateResourceFromFactoryMap(const FilePath& aPath)
	{
		auto itFactory = myFactories.find(typeid(TResource));

		const auto addToOnSetList = [this, &aPath](OnFactorySetList& aList) -> Future<FileResourceRef<TResource>>
		{
			auto [promise, future] = MakeAsync<FileResourceRef<TResource>>();

			aList.emplace_back([this, path = aPath, promise = std::move(promise)]() mutable
				{
					auto& createFuncs = std::get<SCreateFuncs>(myFactories.find(typeid(TResource))->second);

					createFuncs.Create(path)
						.ThenRun(GetExecutor(), FutureRunResult([this, path = std::move(path), promise = std::move(promise)](SharedReference<FileResourceBase> aReference) mutable
							{
								auto castedRef = StaticReferenceCast<TResource>(aReference);

								auto dRef = MakeSharedReference<SharedReference<TResource>>(std::move(castedRef));

								myResources[typeid(TResource)][std::move(path)] = std::make_unique<SResourceData<TResource>>(SResourceData<TResource>
								{
									.WeakPtr		= dRef.Weak(),
									.OnFileChanges	= OnFileResourceUpdate<TResource>(**dRef)
								});

								promise.Complete(FileResourcePtr<TResource>(std::move(dRef)).ToRef());
							}))
						.Detach();
				});

			return std::move(future);
		};

		if (itFactory == myFactories.end())
			itFactory = myFactories.emplace(std::make_pair<std::type_index>(typeid(TResource), OnFactorySetList())).first;

		if (std::holds_alternative<OnFactorySetList>(itFactory->second))
			return addToOnSetList(std::get<OnFactorySetList>(itFactory->second));

		auto& createFuncs = std::get<SCreateFuncs>(itFactory->second);
		
		return createFuncs.Create(aPath)
			.ThenApply(GetExecutor(), FutureApplyResult([this, path = aPath](SharedReference<FileResourceBase> aReference) mutable
				{
					auto castedRef = StaticReferenceCast<TResource>(std::move(aReference));

					auto dRef = MakeSharedReference<SharedReference<TResource>>(std::move(castedRef));

					myResources[typeid(TResource)][path] = std::make_unique<SResourceData<TResource>>(SResourceData<TResource>
					{
						.WeakPtr		= dRef.Weak(),
						.OnFileChanges	= OnFileResourceUpdate<TResource>(**dRef)
					});

					return MakeCompletedFuture<FileResourceRef<TResource>>(FileResourcePtr<TResource>(std::move(dRef)).ToRef());
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
					.OnFileChange(resourcePath, GetExecutor(), Bind(&FileResourceSystem::UpdateResource<TResource>, this, resourcePath)));
			for (const auto& path : relatedFiles)
				subscribers.emplace_back(
					Get<FileWatcherSystem>()
					.OnFileChange(path, GetExecutor(), Bind(&FileResourceSystem::UpdateResource<TResource>, this, resourcePath)));

			return subscribers;
		}
			
	}
	template<CFileResource TResource>
	void UpdateResource(const FilePath& aResourcePath, const FilePath& /*aUpdatedFile*/)
	{
		auto ptrData = GetResourceDataFromMap<TResource>(aResourcePath);

		if (!ptrData)
		{
			WmLogError(TagWonderMake << TagWmResources << "Failed to update resource, unknown resource; path: " << aResourcePath << " resource type: " << GetFileResourceTypeName<TResource>() << '.');

			return;
		}

		auto ptr = ptrData->WeakPtr.lock();

		if (!ptr)
		{
			RemoveResource<TResource>(aResourcePath);

			return;
		}

		auto itFactory = myFactories.find(typeid(TResource));

		auto& createFuncs = std::get<SCreateFuncs>(itFactory->second);

		auto id			= (*ptr)->Id();
		auto generation	= (*ptr)->Generation() + 1;

		WmLogInfo(TagWonderMake << TagWmResources << "Reloading resource; path: " << aResourcePath << " resource type: " << GetFileResourceTypeName<TResource>() << ", id: " << id << ", generation: " << generation << '.');

		createFuncs.CreateWithParams(aResourcePath, id, generation)
			.ThenRun(GetExecutor(), FutureRunResult([this, aResourcePath, weakPtr = ptrData->WeakPtr, id, generation](SharedReference<FileResourceBase> aReference)
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

	ResourceMap	myResources;
	FactoryMap	myFactories;

};
