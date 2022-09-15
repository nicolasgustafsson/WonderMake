#pragma once

#include "wondermake-utility/Executor.h"
#include "wondermake-utility/RestrictTypes.h"
#include "wondermake-utility/UniqueFunction.h"

#include <optional>
#include <mutex>
#include <memory>
#include <type_traits>
#include <vector>

namespace _Impl
{
	enum class EFutureState
	{
		Uncompleted,
		Completed,
		Canceled
	};

	template<typename TType>
	class FutureSharedState final
		: public std::enable_shared_from_this<FutureSharedState<TType>>
	{
	public:
		inline ~FutureSharedState()
		{
			std::unique_lock lock(myMutex);

			if (myState != EFutureState::Uncompleted)
				return;
			
			InlineExecutor executor;

			Resolve(executor, EFutureState::Canceled, std::move(lock), nullptr);
		}

		template<CExecutor TExecutor>
		bool Cancel(TExecutor& aExecutor)
		{
			std::unique_lock lock(myMutex);

			if (myState != EFutureState::Uncompleted)
				return false;

			Resolve(aExecutor, EFutureState::Canceled, std::move(lock), this->shared_from_this());

			return true;
		}
		template<CExecutor TExecutor, typename TArg>
		bool Complete(TExecutor& aExecutor, TArg&& aArg)
		{
			std::unique_lock lock(myMutex);

			if (myState != EFutureState::Uncompleted)
				return false;

			myResult.emplace(std::forward<TArg>(aArg));

			Resolve(aExecutor, EFutureState::Completed, std::move(lock), this->shared_from_this());

			return true;
		}
		EFutureState GetState()
		{
			std::lock_guard<decltype(myMutex)> lock(myMutex);

			return myState;
		}

		template<typename TCallable>
		void Continuation(TCallable&& aContinuation) requires std::is_invocable_v<TCallable, std::shared_ptr<FutureSharedState<TType>>&&>
		{
			{
				std::lock_guard<decltype(myMutex)> lock(myMutex);

				if (myState == EFutureState::Uncompleted)
				{
					myContinuations.emplace_back(std::forward<TCallable>(aContinuation));

					return;
				}
			}

			std::move(aContinuation)(this->shared_from_this());
		}
		template<typename TCallable>
		void Cancellation(TCallable&& aCancellation) requires std::is_invocable_v<TCallable>
		{
			{
				std::lock_guard<decltype(myMutex)> lock(myMutex);

				if (myState == EFutureState::Uncompleted)
				{
					myCancellations.emplace_back(std::forward<TCallable>(aCancellation));

					return;
				}
			}

			std::move(aCancellation)();
		}

		std::optional<TType> GetResult() requires(std::is_copy_constructible_v<TType>)
		{
			std::lock_guard<decltype(myMutex)> lock(myMutex);

			return myResult;
		}
		std::optional<TType> GetResultOwnership()
		{
			std::lock_guard<decltype(myMutex)> lock(myMutex);

			if (!myResult)
				return std::nullopt;

			std::optional<TType> result;

			result.emplace(std::move(*myResult));

			myResult.reset();

			return result;
		}

	private:
		template<CExecutor TExecutor>
		void Resolve(TExecutor& aExecutor, EFutureState aState, std::unique_lock<std::recursive_mutex> aLock, std::shared_ptr<FutureSharedState> aOwningPointer)
		{
			myState = aState;
			
			auto continuations = std::exchange(myContinuations, decltype(myContinuations)());
			auto cancellations = std::exchange(myCancellations, decltype(myCancellations)());

			if (aState == EFutureState::Completed)
				aExecutor.Execute([this, lock = std::move(aLock), continuations = std::move(continuations), owningPointer = std::move(aOwningPointer)]() mutable
					{
						if (!owningPointer || owningPointer.use_count() > 1)
							for (auto&& continuation : continuations)
								std::move(continuation)(this->shared_from_this());

						lock.unlock();

						owningPointer.reset();
					});

			if (aState == EFutureState::Canceled)
				aExecutor.Execute([this, lock = std::move(aLock), cancellations = std::move(cancellations), owningPointer = std::move(aOwningPointer)]() mutable
					{
						if (!owningPointer || owningPointer.use_count() > 1)
							for (auto&& cancellation : cancellations)
								std::move(cancellation)();

						lock.unlock();

						owningPointer.reset();
					});
		}

		std::recursive_mutex myMutex;
		
		EFutureState myState = EFutureState::Uncompleted;
		std::optional<TType> myResult;

		std::vector<UniqueFunction<void(std::shared_ptr<FutureSharedState<TType>>&&)>> myContinuations;
		std::vector<Closure> myCancellations;
	};

	template<>
	class FutureSharedState<void> final
		: public std::enable_shared_from_this<FutureSharedState<void>>
	{
	public:
		inline ~FutureSharedState()
		{
			std::unique_lock lock(myMutex);

			if (myState != EFutureState::Uncompleted)
				return;

			InlineExecutor executor;

			Resolve(executor, EFutureState::Canceled, std::move(lock), nullptr);
		}

		template<CExecutor TExecutor>
		bool Cancel(TExecutor& aExecutor)
		{
			std::unique_lock lock(myMutex);

			if (myState != EFutureState::Uncompleted)
				return false;

			Resolve(aExecutor, EFutureState::Canceled, std::move(lock), this->shared_from_this());

			return true;
		}
		template<CExecutor TExecutor>
		bool Complete(TExecutor& aExecutor)
		{
			std::unique_lock lock(myMutex);

			if (myState != EFutureState::Uncompleted)
				return false;

			Resolve(aExecutor, EFutureState::Completed, std::move(lock), this->shared_from_this());

			return true;
		}
		EFutureState GetState()
		{
			std::lock_guard<decltype(myMutex)> lock(myMutex);

			return myState;
		}

		template<typename TCallable>
		void Continuation(TCallable&& aContinuation) requires std::is_invocable_v<TCallable, std::shared_ptr<FutureSharedState<void>>&&>
		{
			{
				std::lock_guard<decltype(myMutex)> lock(myMutex);

				if (myState == EFutureState::Uncompleted)
				{
					myContinuations.emplace_back(std::forward<TCallable>(aContinuation));

					return;
				}
			}

			std::move(aContinuation)(this->shared_from_this());
		}
		template<typename TCallable>
		void Cancellation(TCallable&& aCancellation) requires std::is_invocable_v<TCallable>
		{
			{
				std::lock_guard<decltype(myMutex)> lock(myMutex);

				if (myState == EFutureState::Uncompleted)
				{
					myCancellations.emplace_back(std::forward<TCallable>(aCancellation));

					return;
				}
			}

			std::move(aCancellation)();
		}

	private:
		template<CExecutor TExecutor>
		void Resolve(TExecutor& aExecutor, EFutureState aState, std::unique_lock<std::recursive_mutex> aLock, std::shared_ptr<FutureSharedState> aOwningPointer)
		{
			myState = aState;
			
			auto continuations = std::exchange(myContinuations, decltype(myContinuations)());
			auto cancellations = std::exchange(myCancellations, decltype(myCancellations)());

			if (aState == EFutureState::Completed)
				aExecutor.Execute([this, lock = std::move(aLock), continuations = std::move(continuations), owningPointer = std::move(aOwningPointer)]() mutable
					{
						if (!owningPointer || owningPointer.use_count() > 1)
							for (auto&& continuation : continuations)
								std::move(continuation)(this->shared_from_this());

						lock.unlock();

						owningPointer.reset();
					});

			if (aState == EFutureState::Canceled)
				aExecutor.Execute([this, lock = std::move(aLock), cancellations = std::move(cancellations), owningPointer = std::move(aOwningPointer)]() mutable
					{
						if (!owningPointer || owningPointer.use_count() > 1)
							for (auto&& cancellation : cancellations)
								std::move(cancellation)();

						lock.unlock();

						owningPointer.reset();
					});
		}

		std::recursive_mutex myMutex;
		
		EFutureState myState = EFutureState::Uncompleted;

		std::vector<UniqueFunction<void(std::shared_ptr<FutureSharedState<void>>&&)>> myContinuations;
		std::vector<Closure> myCancellations;
	};

}

template<typename TType>
class Future;

template<typename TType>
class Promise final
	: NonCopyable
{
public:
	Promise() = delete;
	Promise(std::weak_ptr<_Impl::FutureSharedState<TType>> aSharedState)
		: myState(std::move(aSharedState))
	{}
	Promise(Promise&&) = default;
	~Promise()
	{
		Cancel();
	}

	Promise& operator=(Promise&&) noexcept = default;
	
	[[nodiscard]] bool IsValid() const
	{
		return !myState.expired();
	}

	template<CExecutor TExecutor>
	bool Cancel(TExecutor& aExecutor)
	{
		auto state = myState.lock();

		if (!state)
			return false;

		myState.reset();

		state->Cancel(aExecutor);

		return true;
	}
	bool Cancel()
	{
		InlineExecutor executor;

		return Cancel(executor);
	}
	template<CExecutor TExecutor, typename TArg>
	bool Complete(TExecutor& aExecutor, TArg&& aArg) requires(std::is_constructible_v<TType, decltype(std::forward<TArg>(aArg))>)
	{
		auto state = myState.lock();

		if (!state)
			return false;

		myState.reset();

		state->Complete(aExecutor, std::forward<TArg>(aArg));

		return true;
	}
	template<typename TArg>
	bool Complete(TArg&& aArg)
	{
		InlineExecutor executor;

		return Complete(executor, std::forward<TArg>(aArg));
	}
	
	template<typename TFutureType>
	void AddOwnership(Future<TFutureType> aFuture)
	{
		auto state = myState.lock();

		if (!state)
			return;

		state->Continuation([future = std::move(aFuture)](auto&&) {});
	}

private:
	std::weak_ptr<_Impl::FutureSharedState<TType>> myState;

};

template<>
class Promise<void> final
	: NonCopyable
{
public:
	Promise() = delete;
	Promise(std::weak_ptr<_Impl::FutureSharedState<void>> aSharedState)
		: myState(std::move(aSharedState))
	{}
	Promise(Promise&&) = default;
	~Promise()
	{
		Cancel();
	}

	Promise& operator=(Promise&&) noexcept = default;

	[[nodiscard]] bool IsValid() const
	{
		return !myState.expired();
	}

	template<CExecutor TExecutor>
	bool Cancel(TExecutor& aExecutor)
	{
		auto state = myState.lock();

		if (!state)
			return false;

		myState.reset();

		return state->Cancel(aExecutor);
	}
	bool Cancel()
	{
		InlineExecutor executor;

		return Cancel(executor);
	}
	template<CExecutor TExecutor>
	bool Complete(TExecutor& aExecutor)
	{
		auto state = myState.lock();

		if (!state)
			return false;

		myState.reset();

		return state->Complete(aExecutor);
	}
	bool Complete()
	{
		InlineExecutor executor;

		return Complete(executor);
	}

	template<typename TFutureType>
	void AddOwnership(Future<TFutureType> aFuture)
	{
		auto state = myState.lock();

		if (!state)
			return;

		state->Continuation([future = std::move(aFuture)](auto&&) {});
	}

private:
	std::weak_ptr<_Impl::FutureSharedState<void>> myState;

};

template<typename TType>
class Future final
{
public:
	Future() noexcept = default;
	Future(std::shared_ptr<_Impl::FutureSharedState<TType>> aSharedState) noexcept
		: myState(std::move(aSharedState))
	{}
	Future(const Future&) noexcept = default;
	Future(Future&&) noexcept = default;

	Future& operator=(const Future&) noexcept = default;
	Future& operator=(Future&&) noexcept = default;

	template<CExecutor TExecutor, typename TCallable>
	auto ThenApply(TExecutor&& aExecutor, TCallable&& aCallback) requires (std::is_invocable_v<TCallable, Future<TType>&&> && (std::is_const_v<TType> || std::is_trivial_v<TType>))
	{
		using ReturnType = std::invoke_result_t<TCallable&&, Future<TType>>;

		return ThenApplyHelper(TypeWrapper<ReturnType>(), std::forward<TExecutor>(aExecutor), std::forward<TCallable>(aCallback));
	}

	template<CExecutor TExecutor, typename TCallable>
	auto ThenApply(TExecutor&& aExecutor, TCallable&& aCallback) && requires (std::is_invocable_v<TCallable, Future<TType>&&> && !std::is_const_v<TType> && !std::is_trivial_v<TType>)
	{
		using ReturnType = std::invoke_result_t<TCallable&&, Future<TType>>;

		auto retVal = ThenApplyHelper(TypeWrapper<ReturnType>(), std::forward<TExecutor>(aExecutor), std::forward<TCallable>(aCallback));

		Reset();

		return retVal;
	}
	
	template<CExecutor TExecutor, typename TCallable>
	Future& ThenRun(TExecutor&& aExecutor, TCallable&& aCallback) requires std::is_invocable_v<TCallable, Future<TType>&&>
	{
		if (!myState)
			return *this;

		myState->Continuation([executor = std::forward<TExecutor>(aExecutor), callback = std::forward<TCallable>(aCallback)](auto&& aSharedState) mutable
		{
			executor.Execute([callback = std::move(callback), state = std::move(aSharedState)]() mutable
			{
				(void)std::move(callback)(Future<TType>(std::move(state)));
			});
		});

		return *this;
	}
	
	template<CExecutor TExecutor, typename TCallable>
	Future& OnCancel(TExecutor&& aExecutor, TCallable&& aCallback) requires std::is_invocable_v<TCallable>
	{
		if (!myState)
			return *this;

		myState->Cancellation([executor = std::forward<TExecutor>(aExecutor), callback = std::forward<TCallable>(aCallback)]() mutable
		{
			executor.Execute([callback = std::move(callback)]() mutable
			{
				(void)std::move(callback)();
			});
		});

		return *this;
	}

	[[nodiscard]] Future<std::remove_const_t<TType>> MakeConst() && noexcept requires(!std::is_const_v<TType> && !std::is_trivial_v<TType>)
	{
		return Future<std::remove_const_t<TType>>(std::move(myState));

		myState.reset();
	}

	void Detach()
	{
		myState->Continuation([state = std::move(myState)](auto&&) {});

		Reset();
	}
	void Reset() noexcept
	{
		myState.reset();
	}

	[[nodiscard]] bool IsValid() const noexcept
	{
		return static_cast<bool>(myState);
	}
	[[nodiscard]] bool IsCompleted() const
	{
		if (!myState)
			return false;

		return myState->GetState() == _Impl::EFutureState::Completed;
	}
	[[nodiscard]] bool IsCanceled() const
	{
		if (!myState)
			return false;

		return myState->GetState() == _Impl::EFutureState::Canceled;
	}

	template<typename TFutureType>
	void AddOwnership(Future<TFutureType> aFuture)
	{
		if (!myState)
			return;

		myState->Continuation([future = std::move(aFuture)](auto&&) {});
	}

	std::optional<TType> GetResult() const requires(std::is_trivial_v<TType>)
	{
		if (!myState)
			return std::nullopt;

		return myState->GetResult();
	}
	std::optional<TType> GetResult() const& requires(!std::is_trivial_v<TType> && std::is_copy_constructible_v<TType>)
	{
		if (!myState)
			return std::nullopt;

		return myState->GetResult();
	}
	std::optional<TType> GetResult() && requires(!std::is_trivial_v<TType> && !std::is_const_v<TType>)
	{
		if (!myState)
			return std::nullopt;

		return myState->GetResultOwnership();
	}

private:
	template<typename T>
	struct TypeWrapper {};

	template<typename TNewType, CExecutor TExecutor, typename TCallable>
	auto ThenApplyHelper(TypeWrapper<Future<TNewType>>&&, TExecutor&& aExecutor, TCallable&& aCallback)
	{
		if (!myState)
			return Future<std::decay_t<TNewType>>();

		auto [promise, future] = MakeAsync<std::decay_t<TNewType>>();

		promise.AddOwnership(*this);

		myState->Continuation([executor = std::forward<TExecutor>(aExecutor), callback = std::forward<TCallable>(aCallback), promise = std::move(promise)](auto&& aSharedState) mutable
		{
			executor.Execute([executor, callback = std::move(callback), promise = std::move(promise), state = std::move(aSharedState)]() mutable
			{
				auto future = std::move(callback)(Future<TType>(std::move(state)));

				promise.AddOwnership(future);

				future.ThenRun(std::move(executor), [promise = std::move(promise)](auto&& aFuture) mutable
				{
					if (!aFuture.IsCompleted())
						return;

					if constexpr (std::is_same_v<TNewType, void>)
						promise.Complete();
					else
						promise.Complete(*std::move(aFuture).GetResult());
				});
			});
			
		});

		return future;
	}
	
	std::shared_ptr<_Impl::FutureSharedState<TType>> myState;

};

template<>
class Future<void> final
{
public:
	Future() noexcept = default;
	Future(std::shared_ptr<_Impl::FutureSharedState<void>> aSharedState) noexcept
		: myState(std::move(aSharedState))
	{}
	Future(const Future&) noexcept = default;
	Future(Future&&) noexcept = default;

	Future& operator=(const Future&) noexcept = default;
	Future& operator=(Future&&) noexcept = default;

	template<CExecutor TExecutor, typename TCallable>
	auto ThenApply(TExecutor&& aExecutor, TCallable&& aCallback) requires std::is_invocable_v<TCallable, Future<void>&&>
	{
		using ReturnType = std::invoke_result_t<TCallable&&, Future<void>>;

		return ThenApplyHelper(TypeWrapper<ReturnType>(), std::forward<TExecutor>(aExecutor), std::forward<TCallable>(aCallback));
	}
	
	template<CExecutor TExecutor, typename TCallable>
	Future& ThenRun(TExecutor&& aExecutor, TCallable&& aCallback) requires std::is_invocable_v<TCallable, Future<void>&&>
	{
		if (!myState)
			return *this;

		myState->Continuation([executor = std::forward<TExecutor>(aExecutor), callback = std::forward<TCallable>(aCallback)](auto&& aSharedState) mutable
		{
			executor.Execute([callback = std::move(callback), state = std::move(aSharedState)]() mutable
			{
				(void)std::move(callback)(Future<void>(std::move(state)));
			});
		});

		return *this;
	}
	
	template<CExecutor TExecutor, typename TCallable>
	Future& OnCancel(TExecutor&& aExecutor, TCallable&& aCallback) requires std::is_invocable_v<TCallable>
	{
		if (!myState)
			return *this;

		myState->Cancellation([executor = std::forward<TExecutor>(aExecutor), callback = std::forward<TCallable>(aCallback)]() mutable
		{
			executor.Execute([callback = std::move(callback)]() mutable
			{
				(void)std::move(callback)();
			});
		});

		return *this;
	}

	void Detach()
	{
		myState->Continuation([state = std::move(myState)](auto&&) {});

		Reset();
	}
	void Reset() noexcept
	{
		myState.reset();
	}

	[[nodiscard]] bool IsValid() const noexcept
	{
		return static_cast<bool>(myState);
	}
	[[nodiscard]] bool IsCompleted() const
	{
		if (!myState)
			return false;

		return myState->GetState() == _Impl::EFutureState::Completed;
	}
	[[nodiscard]] bool IsCanceled() const
	{
		if (!myState)
			return false;

		return myState->GetState() == _Impl::EFutureState::Canceled;
	}
	
	template<typename TFutureType>
	void AddOwnership(Future<TFutureType> aFuture)
	{
		if (!myState)
			return;

		myState->Continuation([future = std::move(aFuture)](auto&&) {});
	}

private:
	template<typename T>
	struct TypeWrapper {};

	template<typename TNewType, CExecutor TExecutor, typename TCallable>
	auto ThenApplyHelper(TypeWrapper<Future<TNewType>>&&, TExecutor&& aExecutor, TCallable&& aCallback)
	{
		if (!myState)
			return Future<std::decay_t<TNewType>>();

		auto [promise, future] = MakeAsync<std::decay_t<TNewType>>();

		promise.AddOwnership(*this);

		myState->Continuation([executor = std::forward<TExecutor>(aExecutor), callback = std::forward<TCallable>(aCallback), promise = std::move(promise)](auto&& aSharedState) mutable
		{
			executor.Execute([executor, callback = std::move(callback), promise = std::move(promise), state = std::move(aSharedState)]() mutable
			{
				auto future = std::move(callback)(Future<void>(std::move(state)));

				promise.AddOwnership(future);

				future.ThenRun(std::move(executor), [promise = std::move(promise)](auto&& aFuture) mutable
				{
					if (!aFuture.IsCompleted())
						return;

					if constexpr (std::is_same_v<TNewType, void>)
						promise.Complete();
					else
						promise.Complete(*std::move(aFuture).GetResult());
				});
			});
			
		});

		return future;
	}
	
	std::shared_ptr<_Impl::FutureSharedState<void>> myState;

};

template<typename TType>
auto MakeAsync()
{
	auto sharedState = std::make_shared<_Impl::FutureSharedState<TType>>();

	Promise<TType> promise(sharedState);

	Future<TType> future(std::move(sharedState));

	return std::make_pair(std::move(promise), std::move(future));
}

template<typename TFutureType>
Future<TFutureType> MakeCompletedFuture();

template<typename TFutureType, typename TArg>
inline Future<TFutureType> MakeCompletedFuture(TArg&& aArgs)
{
	auto sharedState = std::make_shared<_Impl::FutureSharedState<TFutureType>>();
	InlineExecutor executor;

	sharedState->Complete(executor, std::forward<TArg>(aArgs));

	return std::move(sharedState);
}

template<>
inline Future<void> MakeCompletedFuture<void>(void)
{
	auto sharedState = std::make_shared<_Impl::FutureSharedState<void>>();
	InlineExecutor executor;

	sharedState->Complete(executor);

	return std::move(sharedState);
}

template<typename... TTypes>
inline [[nodiscard]] Future<void> WaitForAll(Future<TTypes>... aFutures)
{
	class PromiseWrapper
	{
	public:
		inline PromiseWrapper(Promise<void>&& aPromise)
			: myPromise(std::move(aPromise))
		{}
		inline ~PromiseWrapper()
		{
			myPromise.Complete();
		}

		Promise<void> myPromise;
	};

	auto [promise, future] = MakeAsync<void>();

	auto sharedPromise = std::make_shared<PromiseWrapper>(std::move(promise));

	(aFutures.ThenRun(InlineExecutor(), [sharedPromise](auto&&) {}), ...);

	(sharedPromise->myPromise.AddOwnership(std::move(aFutures)), ...);

	return future;
};

template<typename... TTypes>
inline [[nodiscard]] Future<void> WaitForAny(Future<TTypes>... aFutures)
{
	constexpr auto waitFor = [](auto& aFuture, auto& aSharedPromise)
	{
		aFuture.ThenRun(InlineExecutor(), [promise = std::weak_ptr(aSharedPromise)](auto&&)
		{
			auto ptr = promise.lock();

			if (!ptr)
				return;

			ptr->Complete();
		});
		aFuture.OnCancel(InlineExecutor(), [promise = std::weak_ptr(aSharedPromise)]()
		{
			auto ptr = promise.lock();

			if (!ptr)
				return;

			ptr->Complete();
		});
	};
	auto [promise, future] = MakeAsync<void>();

	auto sharedPromise = std::make_shared<Promise<void>>(std::move(promise));

	(waitFor(aFutures, sharedPromise), ...);

	(sharedPromise->AddOwnership(std::move(aFutures)), ...);

	future.ThenRun(InlineExecutor(), [sharedPromise](auto&&) {});

	return future;
};

template<typename TCallable>
inline [[nodiscard]] auto CopyFutureResult(TCallable&& aCallable)
{
	return [callable = std::forward<TCallable>(aCallable)](auto&& aFuture) mutable
	{
		if (!aFuture.IsCompleted())
			return std::decay_t<decltype(std::move(callable)(std::move(*aFuture.GetResult())))>();
		
		auto result = aFuture.GetResult();

		if (!result)
			return std::decay_t<decltype(std::move(callable)(std::move(*result)))>();

		return std::move(callable)(std::move(*result));
	};
};

template<typename TCallable>
inline [[nodiscard]] auto MoveFutureResult(TCallable&& aCallable)
{
	return [callable = std::forward<TCallable>(aCallable)](auto&& aFuture) mutable
	{
		if (!aFuture.IsCompleted())
			return std::decay_t<decltype(std::move(callable)(std::move(*std::move(aFuture).GetResult())))>();

		auto result = std::move(aFuture).GetResult();

		if (!result)
			return std::decay_t<decltype(std::move(callable)(std::move(*result)))>();

		return std::move(callable)(std::move(*result));
	};
};
