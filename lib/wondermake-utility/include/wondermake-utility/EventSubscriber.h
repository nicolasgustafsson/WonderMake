#pragma once

#include "wondermake-utility/Executor.h"
#include "wondermake-utility/Future.h"
#include "wondermake-utility/RestrictTypes.h"

#include <functional>
#include <memory>
#include <type_traits>

namespace _Impl
{
	class EventSharedStateBase
	{
	public:
		inline EventSharedStateBase(
			Promise<void>&& aPromise,
			Future<void>&& aFuture)
			: myCancelPromise(std::move(aPromise))
			, myCancelFuture(std::move(aFuture))
		{}
		inline virtual ~EventSharedStateBase() noexcept
		{
			Cancel(true);
		}

		inline void Cancel(bool aShouldComplete)
		{
			if (IsCanceled())
				return;

			if (aShouldComplete)
				myCancelPromise.Complete();
			else
				myCancelPromise.Cancel();

			myDetachedOwnership.reset();
		}
		inline bool IsCanceled()
		{
			return !myCancelFuture.IsValid()
				|| myCancelFuture.IsCanceled()
				|| myCancelFuture.IsCompleted();
		}
		inline Future<void> OnCancel()
		{
			return myCancelFuture;
		}
		inline void AddDetachedOwnership(std::shared_ptr<EventSharedStateBase> aOwnership)
		{
			if (IsCanceled())
				return;

			myDetachedOwnership = std::move(aOwnership);
		}

	protected:
		Promise<void>							myCancelPromise;
		Future<void>							myCancelFuture;
		std::shared_ptr<EventSharedStateBase>	myDetachedOwnership;

	};

	template<typename TType>
	class EventSharedState final
		: public EventSharedStateBase
		, public std::enable_shared_from_this<EventSharedState<TType>>
	{
	public:
		inline EventSharedState(
			std::function<void(TType)> aCallback,
			Promise<void>&& aPromise,
			Future<void>&& aFuture)
			: EventSharedStateBase(std::move(aPromise), std::move(aFuture))
			, myCallback(std::move(aCallback))
		{}

		template<CExecutor TExecutor, typename TArg>
		inline void Trigger(TExecutor& aExecutor, TArg&& aArgument)
		{
			if (IsCanceled())
				return;

			aExecutor.Execute([self = this->shared_from_this(), arg = std::forward<TArg>(aArgument)]() mutable
				{
					self->myCallback(std::move(arg));
				});
		}

	private:
		std::function<void(TType)> myCallback;

	};

	template<>
	class EventSharedState<void> final
		: public EventSharedStateBase
		, public std::enable_shared_from_this<EventSharedState<void>>
	{
	public:
		inline EventSharedState(
			std::function<void()> aCallback,
			Promise<void>&& aPromise,
			Future<void>&& aFuture)
			: EventSharedStateBase(std::move(aPromise), std::move(aFuture))
			, myCallback(std::move(aCallback))
		{}

		template<CExecutor TExecutor>
		inline void Trigger(TExecutor& aExecutor)
		{
			if (IsCanceled())
				return;

			aExecutor.Execute([self = this->shared_from_this()]()
			{
				self->myCallback();
			});
		}

	private:
		std::function<void()> myCallback;

	};
}

template<typename TType>
class EventTrigger final
	: private NonCopyable
{
public:
	inline EventTrigger(std::weak_ptr<_Impl::EventSharedState<TType>> aSharedState) noexcept
		: myState(std::move(aSharedState))
	{}
	EventTrigger(EventTrigger&&) noexcept = default;
	inline ~EventTrigger()
	{
		auto ptr = myState.lock();

		if (!ptr)
			return;

		ptr->Cancel(false);
	}

	EventTrigger& operator=(EventTrigger&&) noexcept = default;

	template<CExecutor TExecutor, typename TArg>
	inline void Trigger(TExecutor& aExecutor, TArg&& aArgument)
	{
		auto ptr = myState.lock();

		if (!ptr)
			return;

		ptr->Trigger(aExecutor, std::forward<TArg>(aArgument));
	}
	template<typename TArg>
	inline void Trigger(TArg&& aArgument)
	{
		InlineExecutor executor;

		Trigger(executor, std::forward<TArg>(aArgument));
	}
	inline void Cancel()
	{
		auto ptr = myState.lock();

		if (!ptr)
			return;

		ptr->Cancel(true);
	}
	inline Future<void> OnCancel()
	{
		auto ptr = myState.lock();

		if (!ptr)
			return MakeCompletedFuture<void>();

		return ptr->OnCancel();
	}

private:
	std::weak_ptr<_Impl::EventSharedState<TType>> myState;
};

template<>
class EventTrigger<void> final
	: private NonCopyable
{
public:
	inline EventTrigger(std::weak_ptr<_Impl::EventSharedState<void>> aSharedState) noexcept
		: myState(std::move(aSharedState))
	{}
	EventTrigger(EventTrigger&&) noexcept = default;
	inline ~EventTrigger()
	{
		auto ptr = myState.lock();

		if (!ptr)
			return;

		ptr->Cancel(false);
	}

	EventTrigger& operator=(EventTrigger&&) noexcept = default;

	template<CExecutor TExecutor>
	inline void Trigger(TExecutor& aExecutor)
	{
		auto ptr = myState.lock();

		if (!ptr)
			return;

		ptr->Trigger(aExecutor);
	}
	inline void Trigger()
	{
		InlineExecutor executor;

		Trigger(executor);
	}
	inline void Cancel()
	{
		auto ptr = myState.lock();

		if (!ptr)
			return;

		ptr->Cancel(true);
	}
	inline Future<void> OnCancel()
	{
		auto ptr = myState.lock();

		if (!ptr)
			return MakeCompletedFuture<void>();

		return ptr->OnCancel();
	}

private:
	std::weak_ptr<_Impl::EventSharedState<void>> myState;
};

template<typename TType>
void WmOnCancel(EventTrigger<TType>& aCancelable, CExecutor auto&& aExecutor, auto&& aOnCancel)
{
	aCancelable.OnCancel()
		.ThenRun(std::forward<decltype(aExecutor)>(aExecutor), [onCancel = std::move(aOnCancel)](auto&&) { onCancel(); })
		.Detach();
}

class EventSubscriber final
	: private NonCopyable
{
public:
	inline EventSubscriber() noexcept = default;
	EventSubscriber(EventSubscriber&&) noexcept = default;
	inline EventSubscriber(std::shared_ptr<_Impl::EventSharedStateBase> aSharedState) noexcept
		: myState(std::move(aSharedState))
	{}

	EventSubscriber& operator=(EventSubscriber&&) noexcept = default;

	inline [[nodiscard]] bool IsValid() const noexcept
	{
		return static_cast<bool>(myState);
	}
	inline [[nodiscard]] bool IsCanceled() const noexcept
	{
		if (!myState)
			return false;

		return myState->IsCanceled();
	}
	inline Future<void> OnCancel()
	{
		if (!myState)
			return Future<void>();

		return myState->OnCancel();
	}

	inline void Detach() &&
	{
		if (!myState)
			return;

		auto& state = *myState;

		state.AddDetachedOwnership(std::move(myState));

		myState.reset();
	}
	inline void Reset() noexcept
	{
		myState.reset();
	}

private:
	std::shared_ptr<_Impl::EventSharedStateBase> myState;
};

inline void WmOnCancel(EventSubscriber& aCancelable, CExecutor auto&& aExecutor, auto&& aOnCancel)
{
	aCancelable.OnCancel()
		.ThenRun(std::forward<decltype(aExecutor)>(aExecutor), [onCancel = std::move(aOnCancel)](auto&&) { onCancel(); })
		.Detach();
}

template<typename TType, CExecutor TExecutor, typename TCallable>
[[nodiscard]] auto MakeEventTrigger(TExecutor aExecutor, TCallable aCallback) requires(!std::is_void_v<TType>)
{
	auto [promise, future] = MakeAsync<void>();

	auto sharedState = std::make_shared<_Impl::EventSharedState<TType>>([executor = std::move(aExecutor), callback = std::move(aCallback)](TType aArg) mutable
		{
			executor.Execute([callback, arg = std::move(aArg)]() mutable
				{
					std::move(callback)(std::move(arg));
				});
		}, std::move(promise), std::move(future));

	std::weak_ptr<_Impl::EventSharedState<TType>> weak = sharedState;

	return std::make_pair(EventTrigger<TType>(std::move(weak)), EventSubscriber(std::move(sharedState)));
}

template<typename TType, CExecutor TExecutor, typename TCallable>
[[nodiscard]] auto MakeEventTrigger(TExecutor aExecutor, TCallable aCallback) requires(std::is_void_v<TType>)
{
	auto [promise, future] = MakeAsync<void>();

	auto sharedState = std::make_shared<_Impl::EventSharedState<void>>([executor = std::move(aExecutor), callback = std::move(aCallback)]() mutable
		{
			executor.Execute(callback);
		}, std::move(promise), std::move(future));

	std::weak_ptr<_Impl::EventSharedState<void>> weak = sharedState;

	return std::make_pair(EventTrigger<void>(std::move(weak)), EventSubscriber(std::move(sharedState)));
}
