#include "wondermake-tests-common/GTestInclude.h"

#include "ConfigurationTests.h"

#include "wondermake-base/ConfigurationSystem.h"

#include "wondermake-utility-tests/ExecutorMock.h"

template<typename TType>
class OverrideChangedCallable
{
public:
	MOCK_METHOD(void, Executed, (const std::string&, const TType&));

	auto GetCallable()
	{
		return [this](const std::string& aId, const TType& aValue)
		{
			Executed(aId, aValue);
		};
	}
};
class OverrideChangedAnyCallable
{
public:
	MOCK_METHOD(void, Executed, (const std::string&));

	auto GetCallable()
	{
		return [this](const std::string& aId)
		{
			Executed(aId);
		};
	}
};

template<typename TType>
void TestOnOverrideChangedDoesNotCallProvidedFunctor()
{
	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;

	StrictMock<ExecutorMock> executor;
	StrictMock<OverrideChangedAnyCallable> callable;

	executor.DelegateToFake();

	configuration.OnOverrideChanged(executor.GetExecutor(), callable.GetCallable());
}

TEST(ConfigurationSystemTests, onoverridechanged_does_not_call_provided_functor)
{
	TestOnOverrideChangedDoesNotCallProvidedFunctor<bool>();
	TestOnOverrideChangedDoesNotCallProvidedFunctor<u8>();
	TestOnOverrideChangedDoesNotCallProvidedFunctor<u16>();
	TestOnOverrideChangedDoesNotCallProvidedFunctor<u32>();
	TestOnOverrideChangedDoesNotCallProvidedFunctor<u64>();
	TestOnOverrideChangedDoesNotCallProvidedFunctor<i8>();
	TestOnOverrideChangedDoesNotCallProvidedFunctor<i16>();
	TestOnOverrideChangedDoesNotCallProvidedFunctor<i32>();
	TestOnOverrideChangedDoesNotCallProvidedFunctor<i64>();
	TestOnOverrideChangedDoesNotCallProvidedFunctor<f32>();
	TestOnOverrideChangedDoesNotCallProvidedFunctor<f64>();
	TestOnOverrideChangedDoesNotCallProvidedFunctor<std::string>();
	TestOnOverrideChangedDoesNotCallProvidedFunctor<ETestEnumI32>();
	TestOnOverrideChangedDoesNotCallProvidedFunctor<ETestEnumU32>();
}

template<typename TType>
void TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameOverride(const TType& aOverride)
{
	const std::string dummyId = "dummy_id";

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;

	configuration.Set<TType>(dummyId, TType(), EConfigGroup::Application);

	configuration.SetOverride<TType>(dummyId, aOverride);

	StrictMock<ExecutorMock> executor;
	StrictMock<OverrideChangedAnyCallable> callable;

	executor.DelegateToFake();

	configuration.OnOverrideChanged(executor.GetExecutor(), callable.GetCallable())
		.Detach();

	configuration.SetOverride<TType>(dummyId, aOverride);
}

TEST(ConfigurationSystemTests, onoverridechanged_does_not_trigger_event_when_any_override_is_set_to_same_override)
{
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameOverride<bool>(true);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameOverride<u8>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameOverride<u16>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameOverride<u32>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameOverride<u64>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameOverride<i8>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameOverride<i16>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameOverride<i32>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameOverride<i64>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameOverride<f32>(1.f);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameOverride<f64>(1.);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameOverride<std::string>("override");
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameOverride<ETestEnumI32>(ETestEnumI32::One);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameOverride<ETestEnumU32>(ETestEnumU32::One);
}

template<typename TType>
void TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameValue(const TType& aValue)
{
	const std::string dummyId = "dummy_id";

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;

	configuration.Set<TType>(dummyId, aValue, EConfigGroup::Application);

	StrictMock<ExecutorMock> executor;
	StrictMock<OverrideChangedAnyCallable> callable;

	executor.DelegateToFake();

	configuration.OnOverrideChanged(executor.GetExecutor(), callable.GetCallable())
		.Detach();

	configuration.SetOverride<TType>(dummyId, aValue);
}

TEST(ConfigurationSystemTests, onoverridechanged_does_not_trigger_event_when_any_override_is_set_to_same_value)
{
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameValue<bool>(true);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameValue<u8>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameValue<u16>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameValue<u32>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameValue<u64>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameValue<i8>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameValue<i16>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameValue<i32>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameValue<i64>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameValue<f32>(1.f);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameValue<f64>(1.);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameValue<std::string>("override");
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameValue<ETestEnumI32>(ETestEnumI32::One);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsSetToSameValue<ETestEnumU32>(ETestEnumU32::One);
}

template<typename TType>
void TestOnOverrideChangedTriggersEventWhenAnyOverrideIsSet(const TType& aOverride)
{
	const std::string dummyId = "dummy_id";

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;

	configuration.Set<TType>(dummyId, TType(), EConfigGroup::Application);

	StrictMock<ExecutorMock> executor;
	StrictMock<OverrideChangedAnyCallable> callable;

	executor.DelegateToFake();

	configuration.OnOverrideChanged(executor.GetExecutor(), callable.GetCallable())
		.Detach();

	EXPECT_CALL(executor, Execute);
	EXPECT_CALL(callable, Executed(dummyId));

	configuration.SetOverride<TType>(dummyId, aOverride);
}

TEST(ConfigurationSystemTests, onoverridechanged_triggers_event_when_any_override_is_set)
{
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsSet<bool>(true);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsSet<u8>(1);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsSet<u16>(1);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsSet<u32>(1);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsSet<u64>(1);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsSet<i8>(1);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsSet<i16>(1);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsSet<i32>(1);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsSet<i64>(1);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsSet<f32>(1.f);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsSet<f64>(1.);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsSet<std::string>("override");
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsSet<ETestEnumI32>(ETestEnumI32::One);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsSet<ETestEnumU32>(ETestEnumU32::One);
}

template<typename TType>
void TestOnOverrideChangedTriggersEventWhenAnyOverrideIsReset(const TType& aValue)
{
	const std::string dummyId = "dummy_id";

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;

	configuration.Set<TType>(dummyId, aValue, EConfigGroup::Application);

	configuration.SetOverride<TType>(dummyId, TType());

	StrictMock<ExecutorMock> executor;
	StrictMock<OverrideChangedAnyCallable> callable;

	executor.DelegateToFake();

	configuration.OnOverrideChanged(executor.GetExecutor(), callable.GetCallable())
		.Detach();

	EXPECT_CALL(executor, Execute);
	EXPECT_CALL(callable, Executed(dummyId));

	configuration.ResetOverride(dummyId);
}

TEST(ConfigurationSystemTests, onoverridechanged_triggers_event_when_any_override_is_reset)
{
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsReset<bool>(true);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsReset<u8>(1);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsReset<u16>(1);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsReset<u32>(1);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsReset<u64>(1);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsReset<i8>(1);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsReset<i16>(1);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsReset<i32>(1);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsReset<i64>(1);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsReset<f32>(1.f);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsReset<f64>(1.);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsReset<std::string>("override");
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsReset<ETestEnumI32>(ETestEnumI32::One);
	TestOnOverrideChangedTriggersEventWhenAnyOverrideIsReset<ETestEnumU32>(ETestEnumU32::One);
}

template<typename TType>
void TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetToSameValue(const TType& aValue)
{
	const std::string dummyId = "dummy_id";

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;

	configuration.Set<TType>(dummyId, aValue, EConfigGroup::Application);

	configuration.SetOverride<TType>(dummyId, aValue);

	StrictMock<ExecutorMock> executor;
	StrictMock<OverrideChangedAnyCallable> callable;

	executor.DelegateToFake();

	configuration.OnOverrideChanged(executor.GetExecutor(), callable.GetCallable())
		.Detach();

	configuration.ResetOverride(dummyId);
}

TEST(ConfigurationSystemTests, onoverridechanged_does_not_trigger_event_when_any_override_is_reset_to_same_value)
{
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetToSameValue<bool>(true);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetToSameValue<u8>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetToSameValue<u16>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetToSameValue<u32>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetToSameValue<u64>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetToSameValue<i8>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetToSameValue<i16>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetToSameValue<i32>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetToSameValue<i64>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetToSameValue<f32>(1.f);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetToSameValue<f64>(1.);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetToSameValue<std::string>("override");
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetToSameValue<ETestEnumI32>(ETestEnumI32::One);
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetToSameValue<ETestEnumU32>(ETestEnumU32::One);
}

template<typename TType>
void TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetButNoOverrideWasSet()
{
	const std::string dummyId = "dummy_id";

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;

	configuration.Set<TType>(dummyId, TType(), EConfigGroup::Application);

	StrictMock<ExecutorMock> executor;
	StrictMock<OverrideChangedAnyCallable> callable;

	executor.DelegateToFake();

	configuration.OnOverrideChanged(executor.GetExecutor(), callable.GetCallable())
		.Detach();

	configuration.ResetOverride(dummyId);
}

TEST(ConfigurationSystemTests, onoverridechanged_does_not_trigger_event_when_any_override_is_reset_but_no_override_was_set)
{
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetButNoOverrideWasSet<bool>();
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetButNoOverrideWasSet<u8>();
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetButNoOverrideWasSet<u16>();
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetButNoOverrideWasSet<u32>();
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetButNoOverrideWasSet<u64>();
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetButNoOverrideWasSet<i8>();
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetButNoOverrideWasSet<i16>();
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetButNoOverrideWasSet<i32>();
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetButNoOverrideWasSet<i64>();
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetButNoOverrideWasSet<f32>();
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetButNoOverrideWasSet<f64>();
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetButNoOverrideWasSet<std::string>();
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetButNoOverrideWasSet<ETestEnumI32>();
	TestOnOverrideChangedDoesNotTriggerEventWhenAnyOverrideIsResetButNoOverrideWasSet<ETestEnumU32>();
}

template<typename TType>
void TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameOverride(const TType& aOverride)
{
	const std::string dummyId = "dummy_id";

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;

	configuration.Set<TType>(dummyId, TType(), EConfigGroup::Application);

	configuration.SetOverride<TType>(dummyId, aOverride);

	StrictMock<ExecutorMock> executor;
	StrictMock<OverrideChangedCallable<TType>> callable;

	executor.DelegateToFake();

	configuration.OnOverrideChanged<TType>(dummyId, executor.GetExecutor(), callable.GetCallable())
		.Detach();

	configuration.SetOverride<TType>(dummyId, aOverride);
}

TEST(ConfigurationSystemTests, onoverridechanged_does_not_trigger_event_when_specific_override_is_set_to_same_override)
{
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameOverride<bool>(true);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameOverride<u8>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameOverride<u16>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameOverride<u32>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameOverride<u64>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameOverride<i8>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameOverride<i16>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameOverride<i32>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameOverride<i64>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameOverride<f32>(1.f);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameOverride<f64>(1.);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameOverride<std::string>("override");
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameOverride<ETestEnumI32>(ETestEnumI32::One);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameOverride<ETestEnumU32>(ETestEnumU32::One);
}

template<typename TType>
void TestOnOverrideChangedDoesNotTriggerEventWhenOtherOverrideIsSet(const TType& aOverride)
{
	const std::string dummyIdA = "dummy_id_a";
	const std::string dummyIdB = "dummy_id_b";

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;

	configuration.Set<TType>(dummyIdA, TType(), EConfigGroup::Application);
	configuration.Set<TType>(dummyIdB, TType(), EConfigGroup::Application);

	StrictMock<ExecutorMock> executor;
	StrictMock<OverrideChangedCallable<TType>> callable;

	executor.DelegateToFake();

	configuration.OnOverrideChanged<TType>(dummyIdA, executor.GetExecutor(), callable.GetCallable())
		.Detach();

	configuration.SetOverride<TType>(dummyIdB, aOverride);
}

TEST(ConfigurationSystemTests, onoverridechanged_does_not_trigger_event_when_other_override_is_set)
{
	TestOnOverrideChangedDoesNotTriggerEventWhenOtherOverrideIsSet<bool>(true);
	TestOnOverrideChangedDoesNotTriggerEventWhenOtherOverrideIsSet<u8>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenOtherOverrideIsSet<u16>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenOtherOverrideIsSet<u32>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenOtherOverrideIsSet<u64>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenOtherOverrideIsSet<i8>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenOtherOverrideIsSet<i16>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenOtherOverrideIsSet<i32>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenOtherOverrideIsSet<i64>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenOtherOverrideIsSet<f32>(1.f);
	TestOnOverrideChangedDoesNotTriggerEventWhenOtherOverrideIsSet<f64>(1.);
	TestOnOverrideChangedDoesNotTriggerEventWhenOtherOverrideIsSet<std::string>("override");
	TestOnOverrideChangedDoesNotTriggerEventWhenOtherOverrideIsSet<ETestEnumI32>(ETestEnumI32::One);
	TestOnOverrideChangedDoesNotTriggerEventWhenOtherOverrideIsSet<ETestEnumU32>(ETestEnumU32::One);
}

template<typename TType>
void TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameValue(const TType& aValue)
{
	const std::string dummyId = "dummy_id";

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;

	configuration.Set<TType>(dummyId, aValue, EConfigGroup::Application);

	StrictMock<ExecutorMock> executor;
	StrictMock<OverrideChangedCallable<TType>> callable;

	executor.DelegateToFake();

	configuration.OnOverrideChanged<TType>(dummyId, executor.GetExecutor(), callable.GetCallable())
		.Detach();

	configuration.SetOverride<TType>(dummyId, aValue);
}

TEST(ConfigurationSystemTests, onoverridechanged_does_not_trigger_event_when_specific_override_is_set_to_same_value)
{
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameValue<bool>(true);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameValue<u8>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameValue<u16>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameValue<u32>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameValue<u64>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameValue<i8>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameValue<i16>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameValue<i32>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameValue<i64>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameValue<f32>(1.f);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameValue<f64>(1.);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameValue<std::string>("override");
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameValue<ETestEnumI32>(ETestEnumI32::One);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsSetToSameValue<ETestEnumU32>(ETestEnumU32::One);
}

template<typename TType>
void TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsSet(const TType& aOverride)
{
	const std::string dummyId = "dummy_id";

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;

	configuration.Set<TType>(dummyId, TType(), EConfigGroup::Application);

	StrictMock<ExecutorMock> executor;
	StrictMock<OverrideChangedCallable<TType>> callable;

	executor.DelegateToFake();

	configuration.OnOverrideChanged<TType>(dummyId, executor.GetExecutor(), callable.GetCallable())
		.Detach();

	EXPECT_CALL(executor, Execute);
	EXPECT_CALL(callable, Executed(dummyId, aOverride));

	configuration.SetOverride<TType>(dummyId, aOverride);
}

TEST(ConfigurationSystemTests, onoverridechanged_triggers_event_when_specific_override_is_set)
{
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsSet<bool>(true);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsSet<u8>(1);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsSet<u16>(1);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsSet<u32>(1);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsSet<u64>(1);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsSet<i8>(1);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsSet<i16>(1);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsSet<i32>(1);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsSet<i64>(1);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsSet<f32>(1.f);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsSet<f64>(1.);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsSet<std::string>("override");
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsSet<ETestEnumI32>(ETestEnumI32::One);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsSet<ETestEnumU32>(ETestEnumU32::One);
}

template<typename TType>
void TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsReset(const TType& aValue)
{
	const std::string dummyId = "dummy_id";

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;

	configuration.Set<TType>(dummyId, aValue, EConfigGroup::Application);

	configuration.SetOverride<TType>(dummyId, TType());

	StrictMock<ExecutorMock> executor;
	StrictMock<OverrideChangedCallable<TType>> callable;

	executor.DelegateToFake();

	configuration.OnOverrideChanged<TType>(dummyId, executor.GetExecutor(), callable.GetCallable())
		.Detach();

	EXPECT_CALL(executor, Execute);
	EXPECT_CALL(callable, Executed(dummyId, aValue));

	configuration.ResetOverride(dummyId);
}

TEST(ConfigurationSystemTests, onoverridechanged_triggers_event_when_specific_override_is_reset)
{
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsReset<bool>(true);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsReset<u8>(1);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsReset<u16>(1);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsReset<u32>(1);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsReset<u64>(1);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsReset<i8>(1);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsReset<i16>(1);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsReset<i32>(1);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsReset<i64>(1);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsReset<f32>(1.f);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsReset<f64>(1.);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsReset<std::string>("override");
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsReset<ETestEnumI32>(ETestEnumI32::One);
	TestOnOverrideChangedTriggersEventWhenSpecificOverrideIsReset<ETestEnumU32>(ETestEnumU32::One);
}

template<typename TType>
void TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetToSameValue(const TType& aValue)
{
	const std::string dummyId = "dummy_id";

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;

	configuration.Set<TType>(dummyId, aValue, EConfigGroup::Application);

	configuration.SetOverride<TType>(dummyId, aValue);

	StrictMock<ExecutorMock> executor;
	StrictMock<OverrideChangedCallable<TType>> callable;

	executor.DelegateToFake();

	configuration.OnOverrideChanged<TType>(dummyId, executor.GetExecutor(), callable.GetCallable())
		.Detach();

	configuration.ResetOverride(dummyId);
}

TEST(ConfigurationSystemTests, onoverridechanged_does_not_trigger_event_when_specific_override_is_reset_to_same_value)
{
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetToSameValue<bool>(true);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetToSameValue<u8>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetToSameValue<u16>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetToSameValue<u32>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetToSameValue<u64>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetToSameValue<i8>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetToSameValue<i16>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetToSameValue<i32>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetToSameValue<i64>(1);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetToSameValue<f32>(1.f);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetToSameValue<f64>(1.);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetToSameValue<std::string>("override");
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetToSameValue<ETestEnumI32>(ETestEnumI32::One);
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetToSameValue<ETestEnumU32>(ETestEnumU32::One);
}

template<typename TType>
void TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetButNoOverrideWasSet()
{
	const std::string dummyId = "dummy_id";

	ConfigurationSystem::InjectDependencies(std::tie());

	ConfigurationSystem configuration;

	configuration.Set<TType>(dummyId, TType(), EConfigGroup::Application);

	StrictMock<ExecutorMock> executor;
	StrictMock<OverrideChangedCallable<TType>> callable;

	executor.DelegateToFake();

	configuration.OnOverrideChanged<TType>(dummyId, executor.GetExecutor(), callable.GetCallable())
		.Detach();

	configuration.ResetOverride(dummyId);
}

TEST(ConfigurationSystemTests, onoverridechanged_does_not_trigger_event_when_specific_override_is_reset_but_no_override_was_set)
{
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetButNoOverrideWasSet<bool>();
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetButNoOverrideWasSet<u8>();
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetButNoOverrideWasSet<u16>();
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetButNoOverrideWasSet<u32>();
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetButNoOverrideWasSet<u64>();
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetButNoOverrideWasSet<i8>();
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetButNoOverrideWasSet<i16>();
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetButNoOverrideWasSet<i32>();
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetButNoOverrideWasSet<i64>();
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetButNoOverrideWasSet<f32>();
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetButNoOverrideWasSet<f64>();
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetButNoOverrideWasSet<std::string>();
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetButNoOverrideWasSet<ETestEnumI32>();
	TestOnOverrideChangedDoesNotTriggerEventWhenSpecificOverrideIsResetButNoOverrideWasSet<ETestEnumU32>();
}
