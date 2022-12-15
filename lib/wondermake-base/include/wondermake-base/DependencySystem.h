#pragma once

#include "wondermake-base/System.h"
#include "wondermake-base/SystemContainer.h"

#include "wondermake-utility/Bindable.h"
#include "wondermake-utility/CancelableList.h"
#include "wondermake-utility/Future.h"
#include "wondermake-utility/Result.h"
#include "wondermake-utility/SharedReference.h"

class DependencySystem
	: public System<
		Policy::Set<>,
		STrait::Set<
			STWonderMake>>
{
public:
	enum class EFetchError
	{
		Missing,
		InternalError
	};

	void SetSystems(SystemContainer aSystemContainer);
	void ResetSystems();

	template<typename TSystem>
	inline [[nodiscard]] Future<Result<std::reference_wrapper<TSystem>, EFetchError>> Fetch()
	{
		auto [promise, future] = MakeAsync<Result<std::reference_wrapper<TSystem>, EFetchError>>();

		if (mySystems)
		{
			ResolveSystem<TSystem>(std::move(promise));

			return std::move(future);
		}

		myRequests.Emplace(
			SRequestPtr
			{
				std::make_unique<SRequest<TSystem>>(std::move(promise), Bind(&DependencySystem::ResolveSystem<TSystem>, this))
			});

		return std::move(future);
	}

private:
	struct SRequestBase
	{
		virtual ~SRequestBase() noexcept = default;

		virtual void Complete() = 0;
		virtual void OnCancel(AnyExecutor&& aExecutor, Closure&& aOnCancel) = 0;
	};

	template<typename TSystem>
	struct SRequest
		: public SRequestBase
	{
		using PromiseType = Promise<Result<std::reference_wrapper<TSystem>, EFetchError>>;

		using ResolveType = UniqueFunction<void(PromiseType&&)>;

		inline SRequest(PromiseType&& aPromise, ResolveType&& aResolve) noexcept
			: Promise(std::move(aPromise))
			, Resolve(std::move(aResolve))
		{}

		PromiseType							Promise;
		UniqueFunction<void(PromiseType&&)>	Resolve;

		inline void Complete() override
		{
			std::move(Resolve)(std::move(Promise));
		}
		inline void OnCancel(AnyExecutor&& aExecutor, Closure&& aOnCancel) override
		{
			Promise.OnCancel(std::move(aExecutor), std::move(aOnCancel));
		}
	};

	struct SRequestPtr
	{
		std::unique_ptr<SRequestBase> Request;

		inline void OnCancel(AnyExecutor aExecutor, Closure aOnCancel)
		{
			Request->OnCancel(std::move(aExecutor), std::move(aOnCancel));
		}
	};

	template<typename TSystem>
	inline void ResolveSystem(Promise<Result<std::reference_wrapper<TSystem>, EFetchError>>&& aPromise)
	{
		if (!mySystems)
			aPromise.Complete(Err(EFetchError::InternalError));

		auto* system = mySystems->TryGet<TSystem>();

		if (!system)
			aPromise.Complete(Err(EFetchError::Missing));

		aPromise.Complete(Ok(std::reference_wrapper<TSystem>(*system)));
	}

	std::optional<SystemContainer>	mySystems;
	CancelableList<SRequestPtr>		myRequests	= CancelableList<SRequestPtr>(GetExecutor());

};
