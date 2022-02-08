#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wondermake-utility/Bindable.h"
#include "wondermake-utility/UniqueValue.h"

using ::testing::Eq;
using ::testing::Return;
using ::testing::StrictMock;

class CallableMock
{
public:
	MOCK_METHOD(void, ReturnsNothingNoArgs, ());

	MOCK_METHOD(void, ReturnsNothingOneArg, (int a1));
	MOCK_METHOD(void, ReturnsNothingTwoArg, (int a1, int a2));
	MOCK_METHOD(int, ReturnsIntNoArgs, ());

	MOCK_METHOD(void, ReturnsNothingOneNonMovableArg, (UniqueValue<int> a1));
	MOCK_METHOD(void, ReturnsNothingTwoNonMovableArg, (UniqueValue<int> a1, UniqueValue<int> a2));
};

TEST(BindTest, bound_callable_is_called)
{
	StrictMock<CallableMock> mock;

	EXPECT_CALL(mock, ReturnsNothingNoArgs);

	const auto bound = Bind(&CallableMock::ReturnsNothingNoArgs, mock);

	bound();
}

TEST(BindTest, bound_callable_returns_value)
{
	StrictMock<CallableMock> mock;

	EXPECT_CALL(mock, ReturnsIntNoArgs)
		.WillOnce(Return(1234));

	const auto bound = Bind(&CallableMock::ReturnsIntNoArgs, mock);

	EXPECT_EQ(bound(), 1234);
}

TEST(BindTest, bound_callable_is_called_with_one_arg)
{
	StrictMock<CallableMock> mock;

	EXPECT_CALL(mock, ReturnsNothingOneArg(1234));

	const auto bound = Bind(&CallableMock::ReturnsNothingOneArg, mock);

	bound(1234);
}

TEST(BindTest, bound_callable_is_called_with_one_packed_arg)
{
	StrictMock<CallableMock> mock;

	EXPECT_CALL(mock, ReturnsNothingOneArg(1234));

	const auto bound = Bind(&CallableMock::ReturnsNothingOneArg, mock, 1234);

	bound();
}

TEST(BindTest, bound_callable_is_called_with_two_arg)
{
	StrictMock<CallableMock> mock;

	EXPECT_CALL(mock, ReturnsNothingTwoArg(1234, 4567));

	const auto bound = Bind(&CallableMock::ReturnsNothingTwoArg, mock);

	bound(1234, 4567);
}

TEST(BindTest, bound_callable_is_called_with_two_packed_arg)
{
	StrictMock<CallableMock> mock;

	EXPECT_CALL(mock, ReturnsNothingTwoArg(1234, 4567));

	const auto bound = Bind(&CallableMock::ReturnsNothingTwoArg, mock, 1234, 4567);

	bound();
}

TEST(BindTest, bound_callable_is_called_with_one_arg_and_one_packed_arg)
{
	StrictMock<CallableMock> mock;

	EXPECT_CALL(mock, ReturnsNothingTwoArg(1234, 4567));

	const auto bound = Bind(&CallableMock::ReturnsNothingTwoArg, mock, 1234);

	bound(4567);
}

TEST(BindTest, bound_callable_is_called_with_one_unique_arg)
{
	StrictMock<CallableMock> mock;
	UniqueValue<int> val(1234);

	EXPECT_CALL(mock, ReturnsNothingOneNonMovableArg(Eq(std::ref(val))));

	const auto bound = Bind(&CallableMock::ReturnsNothingOneNonMovableArg, mock);

	bound(UniqueValue<int>(1234));
}

TEST(BindTest, bound_callable_is_called_with_one_unique_packed_arg)
{
	StrictMock<CallableMock> mock;
	UniqueValue<int> val(1234);

	EXPECT_CALL(mock, ReturnsNothingOneNonMovableArg(Eq(std::ref(val))));

	auto bound = Bind(&CallableMock::ReturnsNothingOneNonMovableArg, mock, UniqueValue<int>(1234));

	std::move(bound)();
}

TEST(BindTest, bound_callable_is_called_with_two_unique_arg)
{
	StrictMock<CallableMock> mock;
	UniqueValue<int> valA(1234);
	UniqueValue<int> valB(4567);

	EXPECT_CALL(mock, ReturnsNothingTwoNonMovableArg(Eq(std::ref(valA)), Eq(std::ref(valB))));

	auto bound = Bind(&CallableMock::ReturnsNothingTwoNonMovableArg, mock);

	std::move(bound)(UniqueValue<int>(1234), UniqueValue<int>(4567));
}

TEST(BindTest, bound_callable_is_called_with_two_unique_packed_arg)
{
	StrictMock<CallableMock> mock;
	UniqueValue<int> valA(1234);
	UniqueValue<int> valB(4567);

	EXPECT_CALL(mock, ReturnsNothingTwoNonMovableArg(Eq(std::ref(valA)), Eq(std::ref(valB))));

	auto bound = Bind(&CallableMock::ReturnsNothingTwoNonMovableArg, mock, UniqueValue<int>(1234), UniqueValue<int>(4567));

	std::move(bound)();
}

TEST(BindTest, bound_callable_is_called_with_one_unique_arg_and_one_unique_packed_arg)
{
	StrictMock<CallableMock> mock;
	UniqueValue<int> valA(1234);
	UniqueValue<int> valB(4567);

	EXPECT_CALL(mock, ReturnsNothingTwoNonMovableArg(Eq(std::ref(valA)), Eq(std::ref(valB))));

	auto bound = Bind(&CallableMock::ReturnsNothingTwoNonMovableArg, mock, UniqueValue<int>(1234));

	std::move(bound)(UniqueValue<int>(4567));
}

TEST(BindTest, bound_pointer_callable_is_called_with_one_arg)
{
	StrictMock<CallableMock> mock;

	EXPECT_CALL(mock, ReturnsNothingOneArg(1234));

	const auto bound = Bind(&CallableMock::ReturnsNothingOneArg, &mock);

	bound(1234);
}

TEST(BindTest, bound_pointer_callable_is_called_with_one_packed_arg)
{
	StrictMock<CallableMock> mock;

	EXPECT_CALL(mock, ReturnsNothingOneArg(1234));

	const auto bound = Bind(&CallableMock::ReturnsNothingOneArg, &mock, 1234);

	bound();
}

TEST(BindTest, bound_pointer_callable_is_called_with_two_arg)
{
	StrictMock<CallableMock> mock;

	EXPECT_CALL(mock, ReturnsNothingTwoArg(1234, 4567));

	const auto bound = Bind(&CallableMock::ReturnsNothingTwoArg, &mock);

	bound(1234, 4567);
}

TEST(BindTest, bound_pointer_callable_is_called_with_two_packed_arg)
{
	StrictMock<CallableMock> mock;

	EXPECT_CALL(mock, ReturnsNothingTwoArg(1234, 4567));

	const auto bound = Bind(&CallableMock::ReturnsNothingTwoArg, &mock, 1234, 4567);

	bound();
}

TEST(BindTest, bound_pointer_callable_is_called_with_one_arg_and_one_packed_arg)
{
	StrictMock<CallableMock> mock;

	EXPECT_CALL(mock, ReturnsNothingTwoArg(1234, 4567));

	const auto bound = Bind(&CallableMock::ReturnsNothingTwoArg, &mock, 1234);

	bound(4567);
}

TEST(BindTest, bound_pointer_callable_is_called_with_one_unique_arg)
{
	StrictMock<CallableMock> mock;
	UniqueValue<int> val(1234);

	EXPECT_CALL(mock, ReturnsNothingOneNonMovableArg(Eq(std::ref(val))));

	const auto bound = Bind(&CallableMock::ReturnsNothingOneNonMovableArg, &mock);

	bound(UniqueValue<int>(1234));
}

TEST(BindTest, bound_pointer_callable_is_called_with_one_unique_packed_arg)
{
	StrictMock<CallableMock> mock;
	UniqueValue<int> val(1234);

	EXPECT_CALL(mock, ReturnsNothingOneNonMovableArg(Eq(std::ref(val))));

	auto bound = Bind(&CallableMock::ReturnsNothingOneNonMovableArg, &mock, UniqueValue<int>(1234));

	std::move(bound)();
}

TEST(BindTest, bound_pointer_callable_is_called_with_two_unique_arg)
{
	StrictMock<CallableMock> mock;
	UniqueValue<int> valA(1234);
	UniqueValue<int> valB(4567);

	EXPECT_CALL(mock, ReturnsNothingTwoNonMovableArg(Eq(std::ref(valA)), Eq(std::ref(valB))));

	auto bound = Bind(&CallableMock::ReturnsNothingTwoNonMovableArg, &mock);

	std::move(bound)(UniqueValue<int>(1234), UniqueValue<int>(4567));
}

TEST(BindTest, bound_pointer_callable_is_called_with_two_unique_packed_arg)
{
	StrictMock<CallableMock> mock;
	UniqueValue<int> valA(1234);
	UniqueValue<int> valB(4567);

	EXPECT_CALL(mock, ReturnsNothingTwoNonMovableArg(Eq(std::ref(valA)), Eq(std::ref(valB))));

	auto bound = Bind(&CallableMock::ReturnsNothingTwoNonMovableArg, &mock, UniqueValue<int>(1234), UniqueValue<int>(4567));

	std::move(bound)();
}

TEST(BindTest, bound_pointer_callable_is_called_with_one_unique_arg_and_one_unique_packed_arg)
{
	StrictMock<CallableMock> mock;
	UniqueValue<int> valA(1234);
	UniqueValue<int> valB(4567);

	EXPECT_CALL(mock, ReturnsNothingTwoNonMovableArg(Eq(std::ref(valA)), Eq(std::ref(valB))));

	auto bound = Bind(&CallableMock::ReturnsNothingTwoNonMovableArg, &mock, UniqueValue<int>(1234));

	std::move(bound)(UniqueValue<int>(4567));
}

TEST(BindTest, bound_shared_callable_is_called)
{
	auto mock = std::make_shared<StrictMock<CallableMock>>();

	EXPECT_CALL(*mock, ReturnsNothingNoArgs);

	const auto bound = Bind(&CallableMock::ReturnsNothingNoArgs, mock);

	bound();
}

TEST(BindTest, bound_shared_callable_returns_value)
{
	auto mock = std::make_shared<StrictMock<CallableMock>>();

	EXPECT_CALL(*mock, ReturnsIntNoArgs)
		.WillOnce(Return(1234));

	const auto bound = Bind(&CallableMock::ReturnsIntNoArgs, mock);

	EXPECT_EQ(bound(), 1234);
}

TEST(BindTest, bound_shared_callable_is_called_with_one_arg)
{
	auto mock = std::make_shared<StrictMock<CallableMock>>();

	EXPECT_CALL(*mock, ReturnsNothingOneArg(1234));

	const auto bound = Bind(&CallableMock::ReturnsNothingOneArg, mock);

	bound(1234);
}

TEST(BindTest, bound_shared_callable_is_called_with_one_packed_arg)
{
	auto mock = std::make_shared<StrictMock<CallableMock>>();

	EXPECT_CALL(*mock, ReturnsNothingOneArg(1234));

	const auto bound = Bind(&CallableMock::ReturnsNothingOneArg, mock, 1234);

	bound();
}

TEST(BindTest, bound_shared_callable_is_called_with_two_arg)
{
	auto mock = std::make_shared<StrictMock<CallableMock>>();

	EXPECT_CALL(*mock, ReturnsNothingTwoArg(1234, 4567));

	const auto bound = Bind(&CallableMock::ReturnsNothingTwoArg, mock);

	bound(1234, 4567);
}

TEST(BindTest, bound_shared_callable_is_called_with_two_packed_arg)
{
	auto mock = std::make_shared<StrictMock<CallableMock>>();

	EXPECT_CALL(*mock, ReturnsNothingTwoArg(1234, 4567));

	const auto bound = Bind(&CallableMock::ReturnsNothingTwoArg, mock, 1234, 4567);

	bound();
}

TEST(BindTest, bound_shared_callable_is_called_with_one_arg_and_one_packed_arg)
{
	auto mock = std::make_shared<StrictMock<CallableMock>>();

	EXPECT_CALL(*mock, ReturnsNothingTwoArg(1234, 4567));

	const auto bound = Bind(&CallableMock::ReturnsNothingTwoArg, mock, 1234);

	bound(4567);
}

TEST(BindTest, bound_shared_callable_is_called_with_one_unique_arg)
{
	auto mock = std::make_shared<StrictMock<CallableMock>>();
	UniqueValue<int> val(1234);

	EXPECT_CALL(*mock, ReturnsNothingOneNonMovableArg(Eq(std::ref(val))));

	const auto bound = Bind(&CallableMock::ReturnsNothingOneNonMovableArg, mock);

	bound(UniqueValue<int>(1234));
}

TEST(BindTest, bound_shared_callable_is_called_with_one_unique_packed_arg)
{
	auto mock = std::make_shared<StrictMock<CallableMock>>();
	UniqueValue<int> val(1234);

	EXPECT_CALL(*mock, ReturnsNothingOneNonMovableArg(Eq(std::ref(val))));

	auto bound = Bind(&CallableMock::ReturnsNothingOneNonMovableArg, mock, UniqueValue<int>(1234));

	std::move(bound)();
}

TEST(BindTest, bound_shared_callable_is_called_with_two_unique_arg)
{
	auto mock = std::make_shared<StrictMock<CallableMock>>();
	UniqueValue<int> valA(1234);
	UniqueValue<int> valB(4567);

	EXPECT_CALL(*mock, ReturnsNothingTwoNonMovableArg(Eq(std::ref(valA)), Eq(std::ref(valB))));

	auto bound = Bind(&CallableMock::ReturnsNothingTwoNonMovableArg, mock);

	std::move(bound)(UniqueValue<int>(1234), UniqueValue<int>(4567));
}

TEST(BindTest, bound_shared_callable_is_called_with_two_unique_packed_arg)
{
	auto mock = std::make_shared<StrictMock<CallableMock>>();
	UniqueValue<int> valA(1234);
	UniqueValue<int> valB(4567);

	EXPECT_CALL(*mock, ReturnsNothingTwoNonMovableArg(Eq(std::ref(valA)), Eq(std::ref(valB))));

	auto bound = Bind(&CallableMock::ReturnsNothingTwoNonMovableArg, mock, UniqueValue<int>(1234), UniqueValue<int>(4567));

	std::move(bound)();
}

TEST(BindTest, bound_shared_callable_is_called_with_one_unique_arg_and_one_unique_packed_arg)
{
	auto mock = std::make_shared<StrictMock<CallableMock>>();
	UniqueValue<int> valA(1234);
	UniqueValue<int> valB(4567);

	EXPECT_CALL(*mock, ReturnsNothingTwoNonMovableArg(Eq(std::ref(valA)), Eq(std::ref(valB))));

	auto bound = Bind(&CallableMock::ReturnsNothingTwoNonMovableArg, mock, UniqueValue<int>(1234));

	std::move(bound)(UniqueValue<int>(4567));
}

TEST(BindTest, bound_weak_callable_is_called)
{
	auto mock = std::make_shared<StrictMock<CallableMock>>();

	EXPECT_CALL(*mock, ReturnsNothingNoArgs);

	const auto bound = Bind(&CallableMock::ReturnsNothingNoArgs, std::weak_ptr<StrictMock<CallableMock>>(mock));

	bound();
}

TEST(BindTest, bound_weak_callable_is_not_called_when_destroyed)
{
	StrictMock<CallableMock> mock;

	EXPECT_CALL(mock, ReturnsNothingOneArg)
		.Times(0);

	std::shared_ptr<StrictMock<CallableMock>> ptr(&mock, [](auto) {});

	const auto bound = Bind(&CallableMock::ReturnsNothingNoArgs, std::weak_ptr<StrictMock<CallableMock>>(ptr));

	ptr.reset();

	bound();
}

TEST(BindTest, bound_weak_callable_is_called_with_one_arg)
{
	auto mock = std::make_shared<StrictMock<CallableMock>>();

	EXPECT_CALL(*mock, ReturnsNothingOneArg(1234));

	const auto bound = Bind(&CallableMock::ReturnsNothingOneArg, std::weak_ptr<StrictMock<CallableMock>>(mock));

	bound(1234);
}

TEST(BindTest, bound_weak_callable_is_called_with_one_packed_arg)
{
	auto mock = std::make_shared<StrictMock<CallableMock>>();

	EXPECT_CALL(*mock, ReturnsNothingOneArg(1234));

	const auto bound = Bind(&CallableMock::ReturnsNothingOneArg, std::weak_ptr<StrictMock<CallableMock>>(mock), 1234);

	bound();
}

TEST(BindTest, bound_weak_callable_is_called_with_two_arg)
{
	auto mock = std::make_shared<StrictMock<CallableMock>>();

	EXPECT_CALL(*mock, ReturnsNothingTwoArg(1234, 4567));

	const auto bound = Bind(&CallableMock::ReturnsNothingTwoArg, std::weak_ptr<StrictMock<CallableMock>>(mock));

	bound(1234, 4567);
}

TEST(BindTest, bound_weak_callable_is_called_with_two_packed_arg)
{
	auto mock = std::make_shared<StrictMock<CallableMock>>();

	EXPECT_CALL(*mock, ReturnsNothingTwoArg(1234, 4567));

	const auto bound = Bind(&CallableMock::ReturnsNothingTwoArg, std::weak_ptr<StrictMock<CallableMock>>(mock), 1234, 4567);

	bound();
}

TEST(BindTest, bound_weak_callable_is_called_with_one_arg_and_one_packed_arg)
{
	auto mock = std::make_shared<StrictMock<CallableMock>>();

	EXPECT_CALL(*mock, ReturnsNothingTwoArg(1234, 4567));

	const auto bound = Bind(&CallableMock::ReturnsNothingTwoArg, std::weak_ptr<StrictMock<CallableMock>>(mock), 1234);

	bound(4567);
}

TEST(BindTest, bound_weak_callable_is_called_with_one_unique_arg)
{
	auto mock = std::make_shared<StrictMock<CallableMock>>();
	UniqueValue<int> val(1234);

	EXPECT_CALL(*mock, ReturnsNothingOneNonMovableArg(Eq(std::ref(val))));

	const auto bound = Bind(&CallableMock::ReturnsNothingOneNonMovableArg, std::weak_ptr<StrictMock<CallableMock>>(mock));

	bound(UniqueValue<int>(1234));
}

TEST(BindTest, bound_weak_callable_is_called_with_one_unique_packed_arg)
{
	auto mock = std::make_shared<StrictMock<CallableMock>>();
	UniqueValue<int> val(1234);

	EXPECT_CALL(*mock, ReturnsNothingOneNonMovableArg(Eq(std::ref(val))));

	auto bound = Bind(&CallableMock::ReturnsNothingOneNonMovableArg, std::weak_ptr<StrictMock<CallableMock>>(mock), UniqueValue<int>(1234));

	std::move(bound)();
}

TEST(BindTest, bound_weak_callable_is_called_with_two_unique_arg)
{
	auto mock = std::make_shared<StrictMock<CallableMock>>();
	UniqueValue<int> valA(1234);
	UniqueValue<int> valB(4567);

	EXPECT_CALL(*mock, ReturnsNothingTwoNonMovableArg(Eq(std::ref(valA)), Eq(std::ref(valB))));

	auto bound = Bind(&CallableMock::ReturnsNothingTwoNonMovableArg, std::weak_ptr<StrictMock<CallableMock>>(mock));

	std::move(bound)(UniqueValue<int>(1234), UniqueValue<int>(4567));
}

TEST(BindTest, bound_weak_callable_is_called_with_two_unique_packed_arg)
{
	auto mock = std::make_shared<StrictMock<CallableMock>>();
	UniqueValue<int> valA(1234);
	UniqueValue<int> valB(4567);

	EXPECT_CALL(*mock, ReturnsNothingTwoNonMovableArg(Eq(std::ref(valA)), Eq(std::ref(valB))));

	auto bound = Bind(&CallableMock::ReturnsNothingTwoNonMovableArg, std::weak_ptr<StrictMock<CallableMock>>(mock), UniqueValue<int>(1234), UniqueValue<int>(4567));

	std::move(bound)();
}

TEST(BindTest, bound_weak_callable_is_called_with_one_unique_arg_and_one_unique_packed_arg)
{
	auto mock = std::make_shared<StrictMock<CallableMock>>();
	UniqueValue<int> valA(1234);
	UniqueValue<int> valB(4567);

	EXPECT_CALL(*mock, ReturnsNothingTwoNonMovableArg(Eq(std::ref(valA)), Eq(std::ref(valB))));

	auto bound = Bind(&CallableMock::ReturnsNothingTwoNonMovableArg, std::weak_ptr<StrictMock<CallableMock>>(mock), UniqueValue<int>(1234));

	std::move(bound)(UniqueValue<int>(4567));
}
