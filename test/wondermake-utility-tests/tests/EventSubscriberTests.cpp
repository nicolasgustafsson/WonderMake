
#include <gtest/gtest.h>

#include "wondermake-utility-tests/ExecutorMock.h"

#include "wondermake-utility/BufferExecutor.h"
#include "wondermake-utility/EventSubscriber.h"
#include "wondermake-utility/Typedefs.h"

inline constexpr auto locNoOp = [](auto&&...) {};

class SubscriberCallableMock
{
public:
	MOCK_METHOD(void, Executed, ());
	MOCK_METHOD(void, Executed, (u32));

	auto GetCallableVoid()
	{
		return [this]()
		{
			Executed();
		};
	}
	auto GetCallableU32()
	{
		return [this](u32 aValue)
		{
			Executed(aValue);
		};
	}
};

template<CExecutor TExecutor>
class SharedExecutor
{
public:
	inline SharedExecutor(std::shared_ptr<TExecutor> aExecutor)
		: myExecutor(std::move(aExecutor))
	{}

	inline void Execute(auto&& aCallable)
	{
		myExecutor->Execute(std::forward<decltype(aCallable)>(aCallable));
	}

private:
	std::shared_ptr<TExecutor> myExecutor;

};

TEST(EventSubscriberTests, default_constructed_eventsubscriber_is_not_valid)
{
	EventSubscriber subscriber;

	EXPECT_FALSE(subscriber.IsValid());
}

TEST(EventSubscriberTests, default_constructed_eventsubscriber_is_not_canceled)
{
	EventSubscriber subscriber;

	EXPECT_FALSE(subscriber.IsCanceled());
}

TEST(EventSubscriberTests, makeeventtrigger_returns_valid_eventsubscriber)
{
	auto [trigger, subscriber] = MakeEventTrigger<u32>(InlineExecutor(), locNoOp);

	EXPECT_TRUE(subscriber.IsValid());
}

TEST(EventSubscriberTests, makeeventtrigger_returns_valid_void_eventsubscriber)
{
	auto [trigger, subscriber] = MakeEventTrigger<void>(InlineExecutor(), locNoOp);

	EXPECT_TRUE(subscriber.IsValid());
}

TEST(EventSubscriberTests, makeeventtrigger_returns_non_canceled_eventsubscriber)
{
	auto [trigger, subscriber] = MakeEventTrigger<u32>(InlineExecutor(), locNoOp);

	EXPECT_TRUE(subscriber.IsValid());
}

TEST(EventSubscriberTests, makeeventtrigger_returns_non_canceled_void_eventsubscriber)
{
	auto [trigger, subscriber] = MakeEventTrigger<void>(InlineExecutor(), locNoOp);

	EXPECT_FALSE(subscriber.IsCanceled());
}

TEST(EventSubscriberTests, reset_makes_eventsubscriber_invalid)
{
	auto [trigger, subscriber] = MakeEventTrigger<u32>(InlineExecutor(), locNoOp);

	subscriber.Reset();

	EXPECT_FALSE(subscriber.IsCanceled());
}

TEST(EventSubscriberTests, reset_makes_void_eventsubscriber_invalid)
{
	auto [trigger, subscriber] = MakeEventTrigger<void>(InlineExecutor(), locNoOp);

	subscriber.Reset();

	EXPECT_FALSE(subscriber.IsValid());
}

TEST(EventSubscriberTests, trigger_calls_executor)
{
	static constexpr u32 dummyValue = 1234;
	StrictMock<ExecutorMock> executor;

	auto [trigger, subscriber] = MakeEventTrigger<u32>(executor.GetExecutor(), locNoOp);

	EXPECT_CALL(executor, Execute);

	trigger.Trigger(dummyValue);
}

TEST(EventSubscriberTests, void_trigger_calls_executor)
{
	StrictMock<ExecutorMock> executor;

	auto [trigger, subscriber] = MakeEventTrigger<void>(executor.GetExecutor(), locNoOp);

	EXPECT_CALL(executor, Execute);

	trigger.Trigger();
}

TEST(EventSubscriberTests, multiple_calls_to_trigger_calls_executor)
{
	static constexpr u32 dummyValue = 1234;
	StrictMock<ExecutorMock> executor;

	auto [trigger, subscriber] = MakeEventTrigger<u32>(executor.GetExecutor(), locNoOp);

	EXPECT_CALL(executor, Execute);

	trigger.Trigger(dummyValue);

	EXPECT_CALL(executor, Execute);

	trigger.Trigger(dummyValue);

	EXPECT_CALL(executor, Execute);

	trigger.Trigger(dummyValue);
}

TEST(EventSubscriberTests, multiple_calls_to_void_trigger_calls_executor)
{
	StrictMock<ExecutorMock> executor;

	auto [trigger, subscriber] = MakeEventTrigger<void>(executor.GetExecutor(), locNoOp);

	EXPECT_CALL(executor, Execute);

	trigger.Trigger();

	EXPECT_CALL(executor, Execute);

	trigger.Trigger();

	EXPECT_CALL(executor, Execute);

	trigger.Trigger();
}

TEST(EventSubscriberTests, trigger_calls_executor_with_executable)
{
	static constexpr u32 dummyValue = 1234;
	auto executor = std::make_shared<BufferExecutor>();
	StrictMock<SubscriberCallableMock> callable;

	auto [trigger, subscriber] = MakeEventTrigger<u32>(SharedExecutor(executor), callable.GetCallableU32());
	
	trigger.Trigger(dummyValue);
	
	EXPECT_CALL(callable, Executed(dummyValue));
	
	executor->ExecuteAll();
}

TEST(EventSubscriberTests, void_trigger_calls_executor_with_executable)
{
	auto executor = std::make_shared<BufferExecutor>();
	StrictMock<SubscriberCallableMock> callable;

	auto [trigger, subscriber] = MakeEventTrigger<void>(SharedExecutor(executor), callable.GetCallableVoid());

	trigger.Trigger();

	EXPECT_CALL(callable, Executed());

	executor->ExecuteAll();
}

TEST(EventSubscriberTests, trigger_does_not_call_executor_when_eventsubscriber_is_reset)
{
	static constexpr u32 dummyValue = 1234;
	StrictMock<ExecutorMock> executor;

	auto [trigger, subscriber] = MakeEventTrigger<u32>(executor.GetExecutor(), locNoOp);

	subscriber.Reset();

	trigger.Trigger(dummyValue);
}

TEST(EventSubscriberTests, void_trigger_does_not_call_executor_when_eventsubscriber_is_reset)
{
	StrictMock<ExecutorMock> executor;

	auto [trigger, subscriber] = MakeEventTrigger<void>(executor.GetExecutor(), locNoOp);

	subscriber.Reset();

	trigger.Trigger();
}

TEST(EventSubscriberTests, default_constructed_eventsubscriber_oncancel_returns_invalid_future)
{
	EventSubscriber subscriber;

	const auto future = subscriber.OnCancel();

	EXPECT_FALSE(future.IsValid());
}

TEST(EventSubscriberTests, eventsubscriber_oncancel_returns_valid_non_completed_future)
{
	auto [trigger, subscriber] = MakeEventTrigger<u32>(InlineExecutor(), locNoOp);

	const auto future = subscriber.OnCancel();

	EXPECT_TRUE(future.IsValid());
	EXPECT_FALSE(future.IsCompleted());
}

TEST(EventSubscriberTests, void_eventsubscriber_oncancel_returns_valid_non_completed_future)
{
	auto [trigger, subscriber] = MakeEventTrigger<void>(InlineExecutor(), locNoOp);

	const auto future = subscriber.OnCancel();

	EXPECT_TRUE(future.IsValid());
	EXPECT_FALSE(future.IsCompleted());
}

TEST(EventSubscriberTests, eventsubscriber_oncancel_returns_set_future_when_trigger_is_canceled)
{
	auto [trigger, subscriber] = MakeEventTrigger<u32>(InlineExecutor(), locNoOp);

	trigger.Cancel();

	const auto future = subscriber.OnCancel();

	EXPECT_TRUE(future.IsValid());
	EXPECT_TRUE(future.IsCompleted());
}

TEST(EventSubscriberTests, void_eventsubscriber_oncancel_returns_set_future_when_trigger_is_canceled)
{
	auto [trigger, subscriber] = MakeEventTrigger<void>(InlineExecutor(), locNoOp);

	trigger.Cancel();

	const auto future = subscriber.OnCancel();

	EXPECT_TRUE(future.IsValid());
	EXPECT_TRUE(future.IsCompleted());
}

TEST(EventSubscriberTests, eventsubscriber_oncancel_future_is_set_when_trigger_is_canceled)
{
	auto [trigger, subscriber] = MakeEventTrigger<u32>(InlineExecutor(), locNoOp);

	const auto future = subscriber.OnCancel();

	trigger.Cancel();

	EXPECT_TRUE(future.IsValid());
	EXPECT_TRUE(future.IsCompleted());
}

TEST(EventSubscriberTests, void_eventsubscriber_oncancel_future_is_set_when_trigger_is_canceled)
{
	auto [trigger, subscriber] = MakeEventTrigger<void>(InlineExecutor(), locNoOp);

	const auto future = subscriber.OnCancel();

	trigger.Cancel();

	EXPECT_TRUE(future.IsValid());
	EXPECT_TRUE(future.IsCompleted());
}

TEST(EventSubscriberTests, detach_makes_eventsubscriber_invalid)
{
	auto [trigger, subscriber] = MakeEventTrigger<u32>(InlineExecutor(), locNoOp);

	std::move(subscriber).Detach();

	EXPECT_FALSE(subscriber.IsValid());
}

TEST(EventSubscriberTests, void_detach_makes_eventsubscriber_invalid)
{
	auto [trigger, subscriber] = MakeEventTrigger<void>(InlineExecutor(), locNoOp);

	std::move(subscriber).Detach();

	EXPECT_FALSE(subscriber.IsValid());
}

TEST(EventSubscriberTests, trigger_calls_executor_when_eventsubscriber_is_detached)
{
	static constexpr u32 dummyValue = 1234;
	StrictMock<ExecutorMock> executor;

	auto [trigger, subscriber] = MakeEventTrigger<u32>(executor.GetExecutor(), locNoOp);

	std::move(subscriber).Detach();

	EXPECT_CALL(executor, Execute);

	trigger.Trigger(dummyValue);
}

TEST(EventSubscriberTests, void_trigger_calls_executor_when_eventsubscriber_is_detached)
{
	StrictMock<ExecutorMock> executor;

	auto [trigger, subscriber] = MakeEventTrigger<void>(executor.GetExecutor(), locNoOp);

	std::move(subscriber).Detach();

	EXPECT_CALL(executor, Execute);

	trigger.Trigger();
}

TEST(EventSubscriberTests, cancel_makes_iscanceled_return_true)
{
	auto [trigger, subscriber] = MakeEventTrigger<u32>(InlineExecutor(), locNoOp);
	
	trigger.Cancel();

	EXPECT_TRUE(subscriber.IsCanceled());
}

TEST(EventSubscriberTests, void_cancel_makes_iscanceled_return_true)
{
	auto [trigger, subscriber] = MakeEventTrigger<void>(InlineExecutor(), locNoOp);
	
	trigger.Cancel();

	EXPECT_TRUE(subscriber.IsCanceled());
}

TEST(EventSubscriberTests, calling_void_trigger_after_cancel_does_not_call_executor)
{
	StrictMock<ExecutorMock> executor;

	auto [trigger, subscriber] = MakeEventTrigger<void>(executor.GetExecutor(), locNoOp);

	trigger.Cancel();

	trigger.Trigger();
}

TEST(EventSubscriberTests, calling_trigger_after_cancel_does_not_call_executor)
{
	static constexpr u32 dummyValue = 1234;
	StrictMock<ExecutorMock> executor;

	auto [trigger, subscriber] = MakeEventTrigger<u32>(executor.GetExecutor(), locNoOp);
	
	trigger.Cancel();

	trigger.Trigger(dummyValue);
}

TEST(EventSubscriberTests, iscanceled_return_true_when_eventtrigger_is_out_of_scope)
{
	EventSubscriber subscriber;

	{
		auto [trigger, subscriberTemp] = MakeEventTrigger<u32>(InlineExecutor(), locNoOp);

		subscriber = std::move(subscriberTemp);
	}

	EXPECT_TRUE(subscriber.IsCanceled());
}

TEST(EventSubscriberTests, iscanceled_return_true_when_void_eventtrigger_is_out_of_scope)
{
	EventSubscriber subscriber;

	{
		auto [trigger, subscriberTemp] = MakeEventTrigger<void>(InlineExecutor(), locNoOp);

		subscriber = std::move(subscriberTemp);
	}

	EXPECT_TRUE(subscriber.IsCanceled());
}

TEST(EventSubscriberTests, oncancel_eventtrigger_returns_valid_non_completed_future)
{
	auto [trigger, subscriber] = MakeEventTrigger<u32>(InlineExecutor(), locNoOp);

	auto future = trigger.OnCancel();

	EXPECT_TRUE(future.IsValid());
	EXPECT_FALSE(future.IsCompleted());
}

TEST(EventSubscriberTests, void_oncancel_eventtrigger_returns_valid_non_completed_future)
{
	auto [trigger, subscriber] = MakeEventTrigger<void>(InlineExecutor(), locNoOp);

	auto future = trigger.OnCancel();

	EXPECT_TRUE(future.IsValid());
	EXPECT_FALSE(future.IsCompleted());
}

TEST(EventSubscriberTests, oncancel_eventtrigger_sets_future_when_cancel_is_called)
{
	auto [trigger, subscriber] = MakeEventTrigger<u32>(InlineExecutor(), locNoOp);
	
	auto future = trigger.OnCancel();

	trigger.Cancel();

	EXPECT_TRUE(future.IsCompleted());
}

TEST(EventSubscriberTests, void_oncancel_eventtrigger_sets_future_when_cancel_is_called)
{
	auto [trigger, subscriber] = MakeEventTrigger<void>(InlineExecutor(), locNoOp);

	auto future = trigger.OnCancel();

	trigger.Cancel();

	EXPECT_TRUE(future.IsCompleted());
}

TEST(EventSubscriberTests, oncancel_eventtrigger_cancels_future_when_trigger_is_out_of_scope)
{
	auto [trigger, subscriber] = MakeEventTrigger<u32>(InlineExecutor(), locNoOp);

	auto future = trigger.OnCancel();

	{
		auto triggerTemp = std::move(trigger);

		triggerTemp;
	}

	EXPECT_TRUE(future.IsCanceled());
}

TEST(EventSubscriberTests, void_oncancel_eventtrigger_cancels_future_when_trigger_is_out_of_scope)
{
	auto [trigger, subscriber] = MakeEventTrigger<void>(InlineExecutor(), locNoOp);

	auto future = trigger.OnCancel();

	{
		auto triggerTemp = std::move(trigger);

		triggerTemp;
	}

	EXPECT_TRUE(future.IsCanceled());
}

TEST(EventSubscriberTests, oncancel_eventtrigger_sets_future_when_eventsubscriber_is_reset)
{
	auto [trigger, subscriber] = MakeEventTrigger<u32>(InlineExecutor(), locNoOp);

	auto future = trigger.OnCancel();

	subscriber.Reset();

	EXPECT_TRUE(future.IsCompleted());
}

TEST(EventSubscriberTests, void_oncancel_eventtrigger_sets_future_when_eventsubscriber_is_reset)
{
	auto [trigger, subscriber] = MakeEventTrigger<void>(InlineExecutor(), locNoOp);

	auto future = trigger.OnCancel();

	subscriber.Reset();

	EXPECT_TRUE(future.IsCompleted());
}
