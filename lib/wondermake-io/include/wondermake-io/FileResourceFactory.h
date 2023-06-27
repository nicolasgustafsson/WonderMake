#pragma once

#include "wondermake-io/FileResource.h"
#include "wondermake-io/FileResourceProxy.h"
#include "wondermake-io/FileResourceSystem.h"

#include "wondermake-base/GuidGeneratorSystem.h"
#include "wondermake-base/Logger.h"
#include "wondermake-base/System.h"
#include "wondermake-base/WmLogTags.h"

#include "wondermake-utility/Bindable.h"
#include "wondermake-utility/Future.h"
#include "wondermake-utility/SharedReference.h"

#include <memory>
#include <unordered_map>

using FileResourceFactoryPolicySet
	= Policy::Set<
		PAdd<GuidGeneratorSystem, PWrite>,
		PAdd<FileResourceSystem, PWrite>>;
using FileResourceFactoryTraitSet
	= SystemTraits::Set<
		STSingleton>;

template<
	typename TResource,
	typename TPolicySet = Policy::Set<>,
	typename TTraitSet = STrait::Set<>>
class FileResourceFactory
	: public System<
		Policy::Concat<FileResourceFactoryPolicySet, TPolicySet>,
		STrait::Concat<FileResourceFactoryTraitSet, TTraitSet>>
	, public std::enable_shared_from_this<FileResourceFactory<TResource, TPolicySet, TTraitSet>>
{
private:
	using Sys = System<
		Policy::Concat<FileResourceFactoryPolicySet, TPolicySet>,
		STrait::Concat<FileResourceFactoryTraitSet, TTraitSet>>;
	using ThisType = FileResourceFactory<TResource, TPolicySet, TTraitSet>;

public:
	using Resource = TResource;

	void Initialize() override
	{
		auto createFunc				= Bind(&ThisType::CreateResource, this->weak_from_this());
		auto createWithParamsFunc	= Bind(&ThisType::CreateResourceWithParams, this->weak_from_this());

		auto factoryFunc = [createFunc = std::move(createFunc)](FilePath aPath) -> Future<SharedReference<FileResourceBase>>
		{
			auto result = createFunc(std::move(aPath));

			if (!result)
				return MakeCanceledFuture<SharedReference<FileResourceBase>>();

			return std::move(result).Unwrap()
				.ThenApply(InlineExecutor(), FutureApplyResult([](auto aReference)
					{
						return MakeCompletedFuture<SharedReference<FileResourceBase>>(std::move(aReference));
					}));
		};
		auto factoryWithIdFunc = [createFunc = std::move(createWithParamsFunc)](FilePath aPath, FileResourceId aId, u32 aGeneration) -> Future<SharedReference<FileResourceBase>>
		{
			auto result = createFunc(std::move(aPath), aId, aGeneration);

			if (!result)
				return MakeCanceledFuture<SharedReference<FileResourceBase>>();

			return std::move(result).Unwrap()
				.ThenApply(InlineExecutor(), FutureApplyResult([](auto aReference)
					{
						return MakeCompletedFuture<SharedReference<FileResourceBase>>(std::move(aReference));
					}));
		};

		Get<FileResourceSystem>().SetFactory<TResource>(std::move(factoryFunc), std::move(factoryWithIdFunc));
	}

	[[nodiscard]] inline Future<SharedReference<TResource>> CreateResource(FilePath aFilePath)
	{
		auto id = Get<GuidGeneratorSystem>().GenerateNew();

		if (!id)
		{
			WmLogError(TagWonderMake << TagWmResources << "Failed to create resource, guid not generated; resource type: " << GetFileResourceTypeName<TResource>() << '.');

			return MakeCanceledFuture<SharedReference<TResource>>();
		}

		return CreateResourceWithParams(std::move(aFilePath), *id, 0);
	}
	[[nodiscard]] inline Future<SharedReference<TResource>> CreateResourceWithParams(FilePath aFilePath, FileResourceId aId, u32 aGeneration)
	{
		MakeResourceOp op(*this, aId, aFilePath, aGeneration);

		return CreateResourceStrategy(aId, std::move(aFilePath), std::move(op));
	}
	
	template<typename TDependency>
	[[nodiscard]] TDependency& Get()
	{
		return Sys::template Get<TDependency>();
	}
	template<typename TDependency>
	[[nodiscard]] const TDependency& Get() const
	{
		return Sys::template Get<TDependency>();
	}

protected:
	class MakeResourceOp
	{
	public:
		inline MakeResourceOp(
			ThisType&		aThis,
			FileResourceId	aId,
			FilePath		aPath,
			u32				aGeneration)
			: myThis(&aThis)
			, myId(aId)
			, myPath(std::move(aPath))
			, myGeneration(aGeneration)
		{}

		template<typename... TArgs>
		[[nodiscard]] inline SharedReference<TResource> operator()(TArgs&&... aArgs)
		{
			return myThis->MakeResource(myId, myGeneration, std::move(myPath), std::move(myRelatedFiles), std::forward<TArgs>(aArgs)...);
		}

		inline void AddRelatedFile(FilePath aPath)
		{
			myRelatedFiles.emplace(std::move(aPath));
		}

	private:
		ThisType*						myThis;
		FileResourceId					myId;
		FilePath						myPath;
		u32								myGeneration;
		std::unordered_set<FilePath>	myRelatedFiles;

	};

	virtual Future<SharedReference<TResource>> CreateResourceStrategy(FileResourceId aId, FilePath aPath, MakeResourceOp aMakeOp) = 0;
	virtual void DestroyResource(TResource& /*aResource*/) {};
	
private:
	template<typename... TArgs>
	[[nodiscard]] inline SharedReference<TResource> MakeResource(FileResourceId aId, u32 aGeneration, FilePath&& aFilePath, std::unordered_set<FilePath>&& aRelatedFiles, TArgs&&... aArgs)
	{
		FileResourceBase::InjectResourceData(aId, GetFileResourceTypeName<TResource>(), aGeneration, std::move(aFilePath), std::move(aRelatedFiles));

		std::shared_ptr<TResource> ptr(new TResource(std::forward<TArgs>(aArgs)...), [this](TResource* aResource)
			{
				if (!aResource)
					return;

				DestroyResource(*aResource);

				delete aResource;
			});

		return SharedReference<TResource>::FromPointer(std::move(ptr))
			.Unwrap();
	}

};
