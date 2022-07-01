#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-utility/Future.h"
#include "wondermake-utility/Typedefs.h"

template<typename TArg>
class FutureCallbackMock
{
public:
	MOCK_METHOD(void, Invoke, (Future<TArg>&&));

	auto CreateCallback()
	{
		return [this](auto aFuture)
		{
			Invoke(std::move(aFuture));
		};
	}
};

TEST(FutureTests, future_is_valid_returns_false_when_default_constructed)
{
	Future<u32> future;

	EXPECT_FALSE(future.IsValid());
}

TEST(FutureTests, void_future_is_valid_returns_false_when_default_constructed)
{
	Future<void> future;

	EXPECT_FALSE(future.IsValid());
}

TEST(FutureTests, future_is_valid_returns_true_when_made)
{
	auto [promise, future] = MakeAsync<u32>();

	EXPECT_TRUE(future.IsValid());
}

TEST(FutureTests, void_future_is_valid_returns_true_when_made)
{
	auto [promise, future] = MakeAsync<void>();

	EXPECT_TRUE(future.IsValid());
}

TEST(FutureTests, future_is_valid_returns_false_when_reset)
{
	auto [promise, future] = MakeAsync<u32>();

	future.Reset();

	EXPECT_FALSE(future.IsValid());
}

TEST(FutureTests, void_future_is_valid_returns_false_when_reset)
{
	auto [promise, future] = MakeAsync<void>();

	future.Reset();

	EXPECT_FALSE(future.IsValid());
}

TEST(FutureTests, void_futures_are_unfulfilled_when_created)
{
	StrictMock<FutureCallbackMock<void>> callbackMock;

	auto [promise, future] = MakeAsync<void>();

	EXPECT_FALSE(future.IsCompleted());
	EXPECT_FALSE(future.IsCanceled());
}

TEST(FutureTests, callback_void_is_called_when_promise_is_completed)
{
	StrictMock<FutureCallbackMock<void>> callbackMock;

	auto [promise, future] = MakeAsync<void>();

	future.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	EXPECT_CALL(callbackMock, Invoke);

	promise.Complete();

	EXPECT_TRUE(future.IsCompleted());
	EXPECT_FALSE(future.IsCanceled());
}

TEST(FutureTests, callback_void_is_called_when_promise_is_canceled)
{
	StrictMock<FutureCallbackMock<void>> callbackMock;

	auto [promise, future] = MakeAsync<void>();

	future.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	EXPECT_CALL(callbackMock, Invoke);

	promise.Cancel();

	EXPECT_FALSE(future.IsCompleted());
	EXPECT_TRUE(future.IsCanceled());
}

TEST(FutureTests, callback_void_is_called_when_promise_is_resolved_and_future_is_detached)
{
	StrictMock<FutureCallbackMock<void>> callbackMock;

	{
		auto [promise, future] = MakeAsync<void>();

		future
			.ThenRun(InlineExecutor(), callbackMock.CreateCallback())
			.Detach();

		EXPECT_CALL(callbackMock, Invoke);

		promise.Complete();
	}
}

TEST(FutureTests, callback_void_is_called_when_promise_is_destroyed)
{
	StrictMock<FutureCallbackMock<void>> callbackMock;

	{
		auto [promise, future] = MakeAsync<void>();

		future
			.ThenRun(InlineExecutor(), callbackMock.CreateCallback())
			.Detach();

		EXPECT_CALL(callbackMock, Invoke);
	}
}

TEST(FutureTests, callback_void_is_not_called_when_no_future_remain)
{
	StrictMock<FutureCallbackMock<void>> callbackMock;

	{
		auto [promise, future] = MakeAsync<void>();

		future.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

		future.Reset();
	}
}

TEST(FutureTests, make_completed_void_future_returns_a_future_with_completed_state)
{
	auto future = MakeCompletedFuture<void>();

	EXPECT_TRUE(future.IsCompleted());
	EXPECT_FALSE(future.IsCanceled());
}

TEST(FutureTests, thenapply_void_callback_is_not_called_when_returned_future_is_destroyed)
{
	StrictMock<FutureCallbackMock<void>> callbackMock;

	auto [promise, future] = MakeAsync<void>();
	auto [promiseNested, futureNested] = MakeAsync<void>();

	auto futureReturned = future.ThenApply(InlineExecutor(), [&futureNested](auto&&)
		{
			return futureNested;
		});

	futureReturned.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	futureReturned.Reset();
}

TEST(FutureTests, thenapply_void_callback_is_called_when_nested_void_promise_is_completed)
{
	StrictMock<FutureCallbackMock<void>> callbackMock;

	auto [promise, future] = MakeAsync<void>();
	auto [promiseNested, futureNested] = MakeAsync<void>();

	auto futureReturned = future.ThenApply(InlineExecutor(), [&futureNested](auto&&)
		{
			return futureNested;
		});

	futureReturned.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	promise.Complete();

	EXPECT_CALL(callbackMock, Invoke);

	promiseNested.Complete();

	EXPECT_TRUE(futureReturned.IsCompleted());
	EXPECT_FALSE(futureReturned.IsCanceled());
}

TEST(FutureTests, thenapply_void_callback_is_called_when_related_futures_are_destroyed)
{
	StrictMock<FutureCallbackMock<void>> callbackMock;

	auto [promise, future] = MakeAsync<void>();
	auto [promiseNested, futureNested] = MakeAsync<void>();

	auto futureReturned = future.ThenApply(InlineExecutor(), [futureNested = std::move(futureNested)](auto&&) mutable
		{
			return std::move(futureNested);
		});

	future.Reset();

	futureReturned.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	promise.Complete();

	EXPECT_CALL(callbackMock, Invoke);

	promiseNested.Complete();

	EXPECT_TRUE(futureReturned.IsCompleted());
	EXPECT_FALSE(futureReturned.IsCanceled());
}

TEST(FutureTests, thenapply_void_callback_is_passed_correct_result_when_nested_void_promise_is_completed)
{
	constexpr u32 dummyData = 1234;

	NiceMock<FutureCallbackMock<u32>> callbackMock;

	auto [promise, future] = MakeAsync<void>();
	auto [promiseNested, futureNested] = MakeAsync<u32>();

	auto futureReturned = future.ThenApply(InlineExecutor(), [&futureNested](auto&&)
		{
			return futureNested;
		});

	futureReturned.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	promise.Complete();

	promiseNested.Complete(dummyData);

	ASSERT_TRUE(futureReturned.GetResult());

	EXPECT_EQ(*futureReturned.GetResult(), dummyData);
}

TEST(FutureTests, futures_are_unfulfilled_when_created)
{
	StrictMock<FutureCallbackMock<u32>> callbackMock;

	auto [promise, future] = MakeAsync<u32>();

	EXPECT_FALSE(future.IsCompleted());
	EXPECT_FALSE(future.IsCanceled());
	EXPECT_FALSE(future.GetResult());
}

TEST(FutureTests, trivial_callback_is_called_when_promise_is_completed)
{
	StrictMock<FutureCallbackMock<u32>> callbackMock;

	auto [promise, future] = MakeAsync<u32>();

	future.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	EXPECT_CALL(callbackMock, Invoke);

	promise.Complete(0);

	EXPECT_TRUE(future.IsCompleted());
	EXPECT_FALSE(future.IsCanceled());
	EXPECT_TRUE(future.GetResult());
}

TEST(FutureTests, trivial_callback_future_contains_correct_result_when_promise_is_completed)
{
	constexpr u32 dummyData = 1234;

	StrictMock<FutureCallbackMock<u32>> callbackMock;

	auto [promise, future] = MakeAsync<u32>();

	future.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	EXPECT_CALL(callbackMock, Invoke)
		.WillOnce([dummyData](auto&& aFuture)
			{
				auto result = aFuture.GetResult();

				EXPECT_TRUE(result);
				EXPECT_EQ(*result, dummyData);
			});

	promise.Complete(dummyData);

	EXPECT_EQ(*future.GetResult(), dummyData);
}

TEST(FutureTests, trivial_callback_future_contains_result_when_previous_future_takes_ownership_of_the_result)
{
	constexpr u32 dummyData = 1234;

	StrictMock<FutureCallbackMock<u32>> callbackMock;

	auto [promise, future] = MakeAsync<u32>();

	future
		.ThenRun(InlineExecutor(), callbackMock.CreateCallback())
		.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	EXPECT_CALL(callbackMock, Invoke)
		.WillOnce([dummyData](auto&& aFuture)
			{
				auto result = std::move(aFuture).GetResult();

				ASSERT_TRUE(result);

				EXPECT_EQ(*result, dummyData);
			})
		.WillOnce([dummyData](auto&& aFuture)
			{
				auto result = aFuture.GetResult();

				ASSERT_TRUE(result);

				EXPECT_EQ(*result, dummyData);
			});

	promise.Complete(dummyData);

	EXPECT_TRUE(future.IsCompleted());
	EXPECT_FALSE(future.IsCanceled());
	ASSERT_TRUE(future.GetResult());

	EXPECT_EQ(*future.GetResult(), dummyData);
}

TEST(FutureTests, trivial_callback_is_called_when_promise_is_canceled)
{
	StrictMock<FutureCallbackMock<u32>> callbackMock;

	auto [promise, future] = MakeAsync<u32>();

	future.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	EXPECT_CALL(callbackMock, Invoke);

	promise.Cancel();

	EXPECT_FALSE(future.IsCompleted());
	EXPECT_TRUE(future.IsCanceled());
	EXPECT_FALSE(future.GetResult());
}

TEST(FutureTests, trivial_callback_is_called_when_promise_is_resolved_and_future_is_detached)
{
	StrictMock<FutureCallbackMock<u32>> callbackMock;

	{
		auto [promise, future] = MakeAsync<u32>();

		future
			.ThenRun(InlineExecutor(), callbackMock.CreateCallback())
			.Detach();

		EXPECT_CALL(callbackMock, Invoke);

		promise.Complete(0);
	}
}

TEST(FutureTests, trivial_callback_is_called_when_promise_is_destroyed)
{
	StrictMock<FutureCallbackMock<u32>> callbackMock;

	{
		auto [promise, future] = MakeAsync<u32>();

		future
			.ThenRun(InlineExecutor(), callbackMock.CreateCallback())
			.Detach();

		EXPECT_CALL(callbackMock, Invoke);
	}
}

TEST(FutureTests, trivial_future_is_reset_when_detached)
{
	auto [promise, future] = MakeAsync<u32>();
	
	future.Detach();

	EXPECT_FALSE(future.IsValid());
}

TEST(FutureTests, void_future_is_reset_when_detached)
{
	auto [promise, future] = MakeAsync<void>();

	future.Detach();

	EXPECT_FALSE(future.IsValid());
}

TEST(FutureTests, trivial_callback_is_not_called_when_no_future_remain)
{
	StrictMock<FutureCallbackMock<u32>> callbackMock;

	{
		auto [promise, future] = MakeAsync<u32>();

		future.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

		future.Reset();
	}
}

TEST(FutureTests, make_completed_future_returns_a_future_with_completed_state)
{
	constexpr u32 dummyData = 1234;

	auto future = MakeCompletedFuture(dummyData);

	EXPECT_TRUE(future.IsCompleted());
	EXPECT_FALSE(future.IsCanceled());

	ASSERT_TRUE(future.GetResult());

	EXPECT_EQ(*future.GetResult(), dummyData);
}

TEST(FutureTests, trivial_thenapply_callback_is_not_called_when_returned_future_is_destroyed)
{
	StrictMock<FutureCallbackMock<void>> callbackMock;

	auto [promise, future] = MakeAsync<u32>();
	auto [promiseNested, futureNested] = MakeAsync<void>();

	auto futureReturned = future.ThenApply(InlineExecutor(), [&futureNested](auto&&)
		{
			return futureNested;
		});

	futureReturned.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	futureReturned.Reset();

	EXPECT_FALSE(futureReturned.IsCompleted());
	EXPECT_FALSE(futureReturned.IsCanceled());
}

TEST(FutureTests, trivial_thenapply_callback_is_called_when_nested_void_promise_is_completed)
{
	constexpr u32 dummyData = 1234;

	StrictMock<FutureCallbackMock<void>> callbackMock;

	auto [promise, future] = MakeAsync<u32>();
	auto [promiseNested, futureNested] = MakeAsync<void>();

	auto futureReturned = future.ThenApply(InlineExecutor(), [&futureNested](auto&&)
		{
			return futureNested;
		});

	futureReturned.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	promise.Complete(dummyData);

	EXPECT_CALL(callbackMock, Invoke);

	promiseNested.Complete();

	EXPECT_TRUE(futureReturned.IsCompleted());
	EXPECT_FALSE(futureReturned.IsCanceled());
}

TEST(FutureTests, trivial_thenapply_callback_is_called_when_related_futures_are_destroyed)
{
	constexpr u32 dummyData = 1234;

	StrictMock<FutureCallbackMock<void>> callbackMock;

	auto [promise, future] = MakeAsync<u32>();
	auto [promiseNested, futureNested] = MakeAsync<void>();

	auto futureReturned = future.ThenApply(InlineExecutor(), [futureNested = std::move(futureNested)](auto&&) mutable
		{
			return std::move(futureNested);
		});

	future.Reset();

	futureReturned.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	promise.Complete(dummyData);

	EXPECT_CALL(callbackMock, Invoke);

	promiseNested.Complete();

	EXPECT_TRUE(futureReturned.IsCompleted());
	EXPECT_FALSE(futureReturned.IsCanceled());
}

TEST(FutureTests, trivial_thenapply_callback_is_passed_correct_result_when_nested_void_promise_is_completed)
{
	constexpr u32 dummyData = 1234;
	constexpr u32 dummyDataNested = 5678;

	NiceMock<FutureCallbackMock<u32>> callbackMock;

	auto [promise, future] = MakeAsync<u32>();
	auto [promiseNested, futureNested] = MakeAsync<u32>();

	auto futureReturned = future.ThenApply(InlineExecutor(), [&futureNested](auto&&)
		{
			return futureNested;
		});

	futureReturned.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	promise.Complete(dummyData);

	promiseNested.Complete(dummyDataNested);

	ASSERT_TRUE(futureReturned.GetResult());

	EXPECT_EQ(*futureReturned.GetResult(), dummyDataNested);
}

TEST(FutureTests, unique_futures_are_unfulfilled_when_created)
{
	StrictMock<FutureCallbackMock<std::unique_ptr<u32>>> callbackMock;

	auto [promise, future] = MakeAsync<std::unique_ptr<u32>>();

	EXPECT_FALSE(future.IsCompleted());
	EXPECT_FALSE(future.IsCanceled());
}

TEST(FutureTests, unique_callback_is_called_when_promise_is_completed)
{
	StrictMock<FutureCallbackMock<std::unique_ptr<u32>>> callbackMock;

	auto [promise, future] = MakeAsync<std::unique_ptr<u32>>();

	future.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	EXPECT_CALL(callbackMock, Invoke);

	promise.Complete(std::make_unique<u32>(0));

	EXPECT_TRUE(future.IsCompleted());
	EXPECT_FALSE(future.IsCanceled());
	EXPECT_TRUE(std::move(future).GetResult());
}

TEST(FutureTests, unique_callback_future_contains_correct_result_when_promise_is_completed)
{
	constexpr u32 dummyData = 1234;

	StrictMock<FutureCallbackMock<std::unique_ptr<u32>>> callbackMock;

	auto [promise, future] = MakeAsync<std::unique_ptr<u32>>();

	future.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	EXPECT_CALL(callbackMock, Invoke)
		.WillOnce([dummyData](auto&& aFuture)
			{
				auto result = std::move(aFuture).GetResult();

				EXPECT_TRUE(result);
				EXPECT_TRUE(*result);
				EXPECT_EQ(**result, dummyData);
			});

	promise.Complete(std::make_unique<u32>(dummyData));
}

TEST(FutureTests, unique_future_contains_correct_result_when_promise_is_completed)
{
	constexpr u32 dummyResult = 1234;

	NiceMock<FutureCallbackMock<std::unique_ptr<u32>>> callbackMock;

	auto [promise, future] = MakeAsync<std::unique_ptr<u32>>();

	future.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	EXPECT_CALL(callbackMock, Invoke);

	promise.Complete(std::make_unique<u32>(dummyResult));

	auto result = std::move(future).GetResult();

	ASSERT_TRUE(*result);

	ASSERT_TRUE(**result);

	EXPECT_EQ(**result, dummyResult);
}

TEST(FutureTests, unique_callback_future_contains_no_result_when_previous_future_takes_ownership_of_the_result)
{
	constexpr u32 dummyData = 1234;

	StrictMock<FutureCallbackMock<std::unique_ptr<u32>>> callbackMock;

	auto [promise, future] = MakeAsync<std::unique_ptr<u32>>();

	future
		.ThenRun(InlineExecutor(), callbackMock.CreateCallback())
		.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	EXPECT_CALL(callbackMock, Invoke)
		.WillOnce([dummyData](auto&& aFuture)
			{
				auto result = std::move(aFuture).GetResult();

				EXPECT_TRUE(result);
				EXPECT_TRUE(*result);
				EXPECT_EQ(**result, dummyData);
			})
		.WillOnce([dummyData](auto&& aFuture)
			{
				auto result = std::move(aFuture).GetResult();

				EXPECT_FALSE(result);
			});

	promise.Complete(std::make_unique<u32>(dummyData));

	EXPECT_TRUE(future.IsCompleted());
	EXPECT_FALSE(future.IsCanceled());
	EXPECT_FALSE(std::move(future).GetResult());
}

TEST(FutureTests, unique_callback_is_called_when_promise_is_canceled)
{
	StrictMock<FutureCallbackMock<std::unique_ptr<u32>>> callbackMock;

	auto [promise, future] = MakeAsync<std::unique_ptr<u32>>();

	future.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	EXPECT_CALL(callbackMock, Invoke);

	promise.Cancel();

	EXPECT_FALSE(future.IsCompleted());
	EXPECT_TRUE(future.IsCanceled());
	EXPECT_FALSE(std::move(future).GetResult());
}

TEST(FutureTests, unique_callback_is_called_when_promise_is_resolved_and_future_is_detached)
{
	StrictMock<FutureCallbackMock<std::unique_ptr<u32>>> callbackMock;

	{
		auto [promise, future] = MakeAsync<std::unique_ptr<u32>>();

		future
			.ThenRun(InlineExecutor(), callbackMock.CreateCallback())
			.Detach();

		EXPECT_CALL(callbackMock, Invoke);

		promise.Complete(nullptr);
	}
}

TEST(FutureTests, unique_callback_is_called_when_promise_is_destroyed)
{
	StrictMock<FutureCallbackMock<std::unique_ptr<u32>>> callbackMock;

	{
		auto [promise, future] = MakeAsync<std::unique_ptr<u32>>();

		future
			.ThenRun(InlineExecutor(), callbackMock.CreateCallback())
			.Detach();

		EXPECT_CALL(callbackMock, Invoke);
	}
}

TEST(FutureTests, unique_callback_is_not_called_when_no_future_remain)
{
	StrictMock<FutureCallbackMock<std::unique_ptr<u32>>> callbackMock;

	{
		auto [promise, future] = MakeAsync<std::unique_ptr<u32>>();

		future.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

		future.Reset();
	}
}

TEST(FutureTests, make_completed_unique_future_returns_a_future_with_completed_state)
{
	constexpr u32 dummyResult = 1234;

	auto future = MakeCompletedFuture(std::make_unique<u32>(dummyResult));

	ASSERT_TRUE(future.IsCompleted());
	ASSERT_FALSE(future.IsCanceled());

	auto result = std::move(future).GetResult();

	ASSERT_TRUE(*result);

	ASSERT_TRUE(**result);

	EXPECT_EQ(**result, dummyResult);
}

TEST(FutureTests, thenapply_unique_callback_is_not_called_when_returned_future_is_destroyed)
{
	StrictMock<FutureCallbackMock<void>> callbackMock;

	auto [promise, future] = MakeAsync<std::unique_ptr<u32>>();
	auto [promiseNested, futureNested] = MakeAsync<void>();

	auto futureReturned = std::move(future).ThenApply(InlineExecutor(), [&futureNested](auto&&)
			{
				return futureNested;
			});

	futureReturned.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	futureReturned.Reset();

	EXPECT_FALSE(futureReturned.IsCompleted());
	EXPECT_FALSE(futureReturned.IsCanceled());
}

TEST(FutureTests, thenapply_unique_callback_is_called_when_nested_void_promise_is_completed)
{
	constexpr u32 dummyData = 1234;

	StrictMock<FutureCallbackMock<void>> callbackMock;

	auto [promise, future] = MakeAsync<std::unique_ptr<u32>>();
	auto [promiseNested, futureNested] = MakeAsync<void>();

	auto futureReturned = std::move(future).ThenApply(InlineExecutor(), [&futureNested](auto&&)
			{
				return futureNested;
			});

	futureReturned.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	promise.Complete(std::make_unique<u32>(dummyData));

	EXPECT_CALL(callbackMock, Invoke);

	promiseNested.Complete();

	EXPECT_TRUE(futureReturned.IsCompleted());
	EXPECT_FALSE(futureReturned.IsCanceled());
}

TEST(FutureTests, thenapply_unique_callback_is_called_when_related_futures_are_destroyed)
{
	constexpr u32 dummyData = 1234;

	StrictMock<FutureCallbackMock<void>> callbackMock;

	auto [promise, future] = MakeAsync<std::unique_ptr<u32>>();
	auto [promiseNested, futureNested] = MakeAsync<void>();

	auto futureReturned = std::move(future).ThenApply(InlineExecutor(), [futureNested = std::move(futureNested)](auto&&) mutable
	{
		return std::move(futureNested);
	});

	future.Reset();

	futureReturned.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	promise.Complete(std::make_unique<u32>(dummyData));

	EXPECT_CALL(callbackMock, Invoke);

	promiseNested.Complete();

	EXPECT_TRUE(futureReturned.IsCompleted());
	EXPECT_FALSE(futureReturned.IsCanceled());
}

TEST(FutureTests, thenapply_unique_callback_is_passed_correct_result_when_nested_void_promise_is_completed)
{
	constexpr u32 dummyData = 1234;
	constexpr u32 dummyDataNested = 5678;

	NiceMock<FutureCallbackMock<u32>> callbackMock;

	auto [promise, future] = MakeAsync<std::unique_ptr<u32>>();
	auto [promiseNested, futureNested] = MakeAsync<u32>();

	auto futureReturned = std::move(future).ThenApply(InlineExecutor(), [&futureNested](auto&&)
			{
				return futureNested;
			});

	futureReturned.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	promise.Complete(std::make_unique<u32>(dummyData));

	promiseNested.Complete(dummyDataNested);

	ASSERT_TRUE(futureReturned.GetResult());

	EXPECT_EQ(*futureReturned.GetResult(), dummyDataNested);
}

TEST(FutureTests, unique_future_is_reset_when_makeconst_is_called)
{
	auto [promise, future] = MakeAsync<std::unique_ptr<u32>>();
	
	(void)std::move(future).MakeConst();

	EXPECT_FALSE(future.IsValid());
}

TEST(FutureTests, unique_future_is_valid_when_returned_from_makeconst)
{
	auto [promise, future] = MakeAsync<std::unique_ptr<u32>>();

	auto futureConst = std::move(future).MakeConst();

	EXPECT_TRUE(futureConst.IsValid());
}

TEST(FutureTests, unique_future_is_called_when_returned_from_makeconst)
{
	NiceMock<FutureCallbackMock<std::unique_ptr<u32>>> callbackMock;

	auto [promise, future] = MakeAsync<std::unique_ptr<u32>>();

	auto futureConst = std::move(future).MakeConst();

	futureConst.ThenRun(InlineExecutor(), callbackMock.CreateCallback());

	EXPECT_CALL(callbackMock, Invoke);

	promise.Complete(std::make_unique<u32>(1234));
}

TEST(FutureTests, waitforall_is_completed_when_single_future_completes)
{
	auto [promise, future] = MakeAsync<void>();

	auto futureWaitFor = WaitForAll(std::move(future));

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());

	promise.Complete();

	EXPECT_TRUE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());
}

TEST(FutureTests, waitforall_is_not_completed_when_first_future_completes)
{
	auto [promiseA, futureA] = MakeAsync<void>();
	auto [promiseB, futureB] = MakeAsync<void>();
	auto [promiseC, futureC] = MakeAsync<void>();

	auto futureWaitFor = WaitForAll(std::move(futureA), std::move(futureB), std::move(futureC));

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());

	promiseA.Complete();

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());
}

TEST(FutureTests, waitforall_is_not_completed_when_second_future_completes)
{
	auto [promiseA, futureA] = MakeAsync<void>();
	auto [promiseB, futureB] = MakeAsync<void>();
	auto [promiseC, futureC] = MakeAsync<void>();

	auto futureWaitFor = WaitForAll(std::move(futureA), std::move(futureB), std::move(futureC));

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());

	promiseB.Complete();

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());
}

TEST(FutureTests, waitforall_is_not_completed_when_third_future_completes)
{
	auto [promiseA, futureA] = MakeAsync<void>();
	auto [promiseB, futureB] = MakeAsync<void>();
	auto [promiseC, futureC] = MakeAsync<void>();

	auto futureWaitFor = WaitForAll(std::move(futureA), std::move(futureB), std::move(futureC));

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());

	promiseC.Complete();

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());
}

TEST(FutureTests, waitforall_is_not_completed_when_two_futures_completes)
{
	auto [promiseA, futureA] = MakeAsync<void>();
	auto [promiseB, futureB] = MakeAsync<void>();
	auto [promiseC, futureC] = MakeAsync<void>();

	auto futureWaitFor = WaitForAll(std::move(futureA), std::move(futureB), std::move(futureC));

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());

	promiseA.Complete();
	promiseC.Complete();

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());
}

TEST(FutureTests, waitforall_is_completed_when_all_futures_completes)
{
	auto [promiseA, futureA] = MakeAsync<void>();
	auto [promiseB, futureB] = MakeAsync<void>();
	auto [promiseC, futureC] = MakeAsync<void>();

	auto futureWaitFor = WaitForAll(std::move(futureA), std::move(futureB), std::move(futureC));

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());

	promiseA.Complete();
	promiseB.Complete();
	promiseC.Complete();

	EXPECT_TRUE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());
}

TEST(FutureTests, waitforall_is_completed_when_single_future_is_canceled)
{
	auto [promise, future] = MakeAsync<void>();

	auto futureWaitFor = WaitForAll(std::move(future));

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());

	promise.Cancel();

	EXPECT_TRUE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());
}

TEST(FutureTests, waitforall_is_not_completed_when_first_future_is_canceled)
{
	auto [promiseA, futureA] = MakeAsync<void>();
	auto [promiseB, futureB] = MakeAsync<void>();
	auto [promiseC, futureC] = MakeAsync<void>();

	auto futureWaitFor = WaitForAll(std::move(futureA), std::move(futureB), std::move(futureC));

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());

	promiseA.Complete();

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());
}

TEST(FutureTests, waitforall_is_not_completed_when_second_future_is_canceled)
{
	auto [promiseA, futureA] = MakeAsync<void>();
	auto [promiseB, futureB] = MakeAsync<void>();
	auto [promiseC, futureC] = MakeAsync<void>();

	auto futureWaitFor = WaitForAll(std::move(futureA), std::move(futureB), std::move(futureC));

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());

	promiseB.Complete();

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());
}

TEST(FutureTests, waitforall_is_not_completed_when_third_future_is_canceled)
{
	auto [promiseA, futureA] = MakeAsync<void>();
	auto [promiseB, futureB] = MakeAsync<void>();
	auto [promiseC, futureC] = MakeAsync<void>();

	auto futureWaitFor = WaitForAll(std::move(futureA), std::move(futureB), std::move(futureC));

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());

	promiseC.Complete();

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());
}

TEST(FutureTests, waitforall_is_not_completed_when_two_futures_is_canceled)
{
	auto [promiseA, futureA] = MakeAsync<void>();
	auto [promiseB, futureB] = MakeAsync<void>();
	auto [promiseC, futureC] = MakeAsync<void>();

	auto futureWaitFor = WaitForAll(std::move(futureA), std::move(futureB), std::move(futureC));

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());

	promiseA.Complete();
	promiseC.Complete();

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());
}

TEST(FutureTests, waitforall_is_completed_when_all_futures_is_canceled)
{
	auto [promiseA, futureA] = MakeAsync<void>();
	auto [promiseB, futureB] = MakeAsync<void>();
	auto [promiseC, futureC] = MakeAsync<void>();

	auto futureWaitFor = WaitForAll(std::move(futureA), std::move(futureB), std::move(futureC));

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());

	promiseA.Complete();
	promiseB.Complete();
	promiseC.Complete();

	EXPECT_TRUE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());
}

TEST(FutureTests, waitforall_is_completed_when_all_futures_are_either_completed_or_canceled)
{
	auto [promiseA, futureA] = MakeAsync<void>();
	auto [promiseB, futureB] = MakeAsync<void>();
	auto [promiseC, futureC] = MakeAsync<void>();

	auto futureWaitFor = WaitForAll(std::move(futureA), std::move(futureB), std::move(futureC));

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());

	promiseA.Complete();
	promiseB.Cancel();
	promiseC.Complete();

	EXPECT_TRUE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());
}

TEST(FutureTests, waitforany_is_completed_when_single_future_completes)
{
	auto [promise, future] = MakeAsync<void>();
	
	auto futureWaitFor = WaitForAny(std::move(future));

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());

	promise.Complete();

	EXPECT_TRUE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());
}

TEST(FutureTests, waitforany_is_completed_when_first_future_completes)
{
	auto [promiseA, futureA] = MakeAsync<void>();
	auto [promiseB, futureB] = MakeAsync<void>();
	auto [promiseC, futureC] = MakeAsync<void>();

	auto futureWaitFor = WaitForAny(std::move(futureA), std::move(futureB), std::move(futureC));

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());

	promiseA.Complete();

	EXPECT_TRUE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());
}

TEST(FutureTests, waitforany_is_completed_when_second_future_completes)
{
	auto [promiseA, futureA] = MakeAsync<void>();
	auto [promiseB, futureB] = MakeAsync<void>();
	auto [promiseC, futureC] = MakeAsync<void>();

	auto futureWaitFor = WaitForAny(std::move(futureA), std::move(futureB), std::move(futureC));

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());

	promiseB.Complete();

	EXPECT_TRUE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());
}

TEST(FutureTests, waitforany_is_completed_when_third_future_completes)
{
	auto [promiseA, futureA] = MakeAsync<void>();
	auto [promiseB, futureB] = MakeAsync<void>();
	auto [promiseC, futureC] = MakeAsync<void>();

	auto futureWaitFor = WaitForAny(std::move(futureA), std::move(futureB), std::move(futureC));

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());

	promiseC.Complete();

	EXPECT_TRUE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());
}

TEST(FutureTests, waitforany_is_completed_when_single_future_is_canceled)
{
	auto [promise, future] = MakeAsync<void>();

	auto futureWaitFor = WaitForAny(std::move(future));

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());

	promise.Cancel();

	EXPECT_TRUE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());
}

TEST(FutureTests, waitforany_is_completed_when_first_future_is_canceled)
{
	auto [promiseA, futureA] = MakeAsync<void>();
	auto [promiseB, futureB] = MakeAsync<void>();
	auto [promiseC, futureC] = MakeAsync<void>();

	auto futureWaitFor = WaitForAny(std::move(futureA), std::move(futureB), std::move(futureC));

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());

	promiseA.Cancel();

	EXPECT_TRUE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());
}

TEST(FutureTests, waitforany_is_completed_when_second_future_is_canceled)
{
	auto [promiseA, futureA] = MakeAsync<void>();
	auto [promiseB, futureB] = MakeAsync<void>();
	auto [promiseC, futureC] = MakeAsync<void>();

	auto futureWaitFor = WaitForAny(std::move(futureA), std::move(futureB), std::move(futureC));

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());

	promiseB.Cancel();

	EXPECT_TRUE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());
}

TEST(FutureTests, waitforany_is_completed_when_third_future_is_canceled)
{
	auto [promiseA, futureA] = MakeAsync<void>();
	auto [promiseB, futureB] = MakeAsync<void>();
	auto [promiseC, futureC] = MakeAsync<void>();

	auto futureWaitFor = WaitForAny(std::move(futureA), std::move(futureB), std::move(futureC));

	EXPECT_FALSE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());

	promiseC.Cancel();

	EXPECT_TRUE(futureWaitFor.IsCompleted());
	EXPECT_FALSE(futureWaitFor.IsCanceled());
}
