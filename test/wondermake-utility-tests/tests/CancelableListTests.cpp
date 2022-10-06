
#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-utility/CancelableList.h"
#include "wondermake-utility/Future.h"
#include "wondermake-utility/Typedefs.h"
#include "wondermake-utility/UniqueFunction.h"

inline constexpr auto locNoOp = [](auto&&...) {};

struct CancelableStateMock
{
	inline CancelableStateMock()
	{
		ON_CALL(*this, OnCancel)
			.WillByDefault([this](auto aOnCancel)
				{
					myOnCancel = std::move(aOnCancel);
				});
	}

	MOCK_METHOD(void, OnCancel, (Closure));

	void TriggerOnCancel()
	{
		std::move(myOnCancel)();
	}

private:
	Closure myOnCancel = locNoOp;

};

class CancelableMock
{
public:
	inline CancelableMock(CancelableStateMock& aState, u32 aValue)
		: myState(&aState)
		, Value(aValue)
	{}
	inline CancelableMock(u32 aValue)
		: myState(nullptr)
		, Value(aValue)
	{}

	void OnCancel(CExecutor auto&& aExecutor, auto&& aOnCancel)
	{
		if (!myState)
			return;

		myState->OnCancel([executor = std::forward<decltype(aExecutor)>(aExecutor), onCancel = std::forward<decltype(aOnCancel)>(aOnCancel)]() mutable
		{
			executor.Execute(std::move(onCancel));
		});
	}

	u32 Value;

private:
	CancelableStateMock* myState;

};

TEST(CancelableListTests, default_constructed_cancelable_queue_is_empty)
{
	auto queue = CancelableList<CancelableMock>(InlineExecutor());

	EXPECT_TRUE(queue.IsEmpty());
}

TEST(CancelableListTests, cancelablelist_is_not_empty_after_cancelable_is_emplaced)
{
	auto queue = CancelableList<CancelableMock>(InlineExecutor());

	queue.Emplace(CancelableMock(1234));

	EXPECT_FALSE(queue.IsEmpty());
}

TEST(CancelableListTests, cancelablelist_is_not_empty_after_a_oncancel_is_triggered_when_more_cancelable_are_left)
{
	static constexpr u32 dummyValue = 1234;

	auto queue = CancelableList<CancelableMock>(InlineExecutor());
	NiceMock<CancelableStateMock> mock;

	queue.Emplace(CancelableMock(mock, dummyValue));
	queue.Emplace(CancelableMock(dummyValue));

	mock.TriggerOnCancel();

	EXPECT_FALSE(queue.IsEmpty());
}

TEST(CancelableListTests, clear_removes_all_emplaced_cancelables)
{
	auto queue = CancelableList<CancelableMock>(InlineExecutor());

	queue.Emplace(CancelableMock(1234));
	queue.Emplace(CancelableMock(2345));
	queue.Emplace(CancelableMock(3456));

	queue.Clear();

	EXPECT_TRUE(queue.IsEmpty());
}

TEST(CancelableListTests, pop_returns_emplaced_value)
{
	static constexpr u32 dummyValueA = 1234;
	static constexpr u32 dummyValueB = 2345;
	static constexpr u32 dummyValueC = 3456;

	auto queue = CancelableList<CancelableMock>(InlineExecutor());

	queue.Emplace(CancelableMock(dummyValueA));
	
	queue.Emplace(CancelableMock(dummyValueB));

	queue.Emplace(CancelableMock(dummyValueC));

	EXPECT_EQ(queue.Pop().Value, dummyValueA);

	EXPECT_EQ(queue.Pop().Value, dummyValueB);

	EXPECT_EQ(queue.Pop().Value, dummyValueC);
}

TEST(CancelableListTests, pop_returns_next_uncanceled_value)
{
	static constexpr u32 dummyValueA = 1234;
	static constexpr u32 dummyValueB = 2345;

	auto queue = CancelableList<CancelableMock>(InlineExecutor());
	NiceMock<CancelableStateMock> mock;

	queue.Emplace(CancelableMock(mock, dummyValueA));

	queue.Emplace(CancelableMock(dummyValueB));

	mock.TriggerOnCancel();

	EXPECT_EQ(queue.Pop().Value, dummyValueB);
}

TEST(CancelableListTests, iterator_begin_end_is_same_when_empty)
{
	auto queue = CancelableList<CancelableMock>(InlineExecutor());

	EXPECT_EQ(queue.begin(), queue.end());
}

TEST(CancelableListTests, iterator_begin_end_is_not_same_when_not_empty)
{
	auto queue = CancelableList<CancelableMock>(InlineExecutor());

	queue.Emplace(CancelableMock(1234));

	EXPECT_NE(queue.begin(), queue.end());
}

TEST(CancelableListTests, iterator_begin_end_is_same_when_incremented)
{
	auto queue = CancelableList<CancelableMock>(InlineExecutor());

	queue.Emplace(CancelableMock(1234));

	auto it = queue.begin();

	++it;

	EXPECT_EQ(it, queue.end());
}

TEST(CancelableListTests, increment_iterator_is_same_as_begin)
{
	auto queue = CancelableList<CancelableMock>(InlineExecutor());

	queue.Emplace(CancelableMock(1234));

	auto it = queue.begin();

	it = it++;

	EXPECT_EQ(it, queue.begin());
}

TEST(CancelableListTests, dereferenced_iterator_has_correct_value)
{
	static constexpr u32 dummyValue = 1234;

	auto queue = CancelableList<CancelableMock>(InlineExecutor());

	queue.Emplace(CancelableMock(dummyValue));

	EXPECT_EQ((*queue.begin()).Value, dummyValue);
	EXPECT_EQ(queue.begin()->Value, dummyValue);
}

TEST(CancelableListTests, dereferenced_iterator_can_be_changed)
{
	static constexpr u32 dummyValueA = 1234;
	static constexpr u32 dummyValueB = 2345;

	auto queue = CancelableList<CancelableMock>(InlineExecutor());

	queue.Emplace(CancelableMock(dummyValueA));

	(*queue.begin()).Value = dummyValueB;

	EXPECT_EQ((*queue.begin()).Value, dummyValueB);
	EXPECT_EQ(queue.begin()->Value, dummyValueB);
}

TEST(CancelableListTests, pointer_dereferenced_iterator_can_be_changed)
{
	static constexpr u32 dummyValueA = 1234;
	static constexpr u32 dummyValueB = 2345;

	auto queue = CancelableList<CancelableMock>(InlineExecutor());

	queue.Emplace(CancelableMock(dummyValueA));

	queue.begin()->Value = dummyValueB;

	EXPECT_EQ((*queue.begin()).Value, dummyValueB);
	EXPECT_EQ(queue.begin()->Value, dummyValueB);
}

const auto& AsConst(const auto& aValue)
{
	return aValue;
}

TEST(CancelableListTests, const_queue_iterator_begin_end_is_same_when_empty)
{
	auto queue = CancelableList<CancelableMock>(InlineExecutor());

	EXPECT_EQ(AsConst(queue).begin(), AsConst(queue).end());
}

TEST(CancelableListTests, const_queue_iterator_begin_end_is_not_same_when_not_empty)
{
	auto queue = CancelableList<CancelableMock>(InlineExecutor());

	queue.Emplace(CancelableMock(1234));

	EXPECT_NE(AsConst(queue).begin(), AsConst(queue).end());
}

TEST(CancelableListTests, const_queue_iterator_begin_end_is_same_when_incremented)
{
	auto queue = CancelableList<CancelableMock>(InlineExecutor());

	queue.Emplace(CancelableMock(1234));

	auto it = AsConst(queue).begin();

	++it;

	EXPECT_EQ(it, AsConst(queue).end());
}

TEST(CancelableListTests, increment_const_queue_iterator_is_same_as_begin)
{
	auto queue = CancelableList<CancelableMock>(InlineExecutor());

	queue.Emplace(CancelableMock(1234));

	auto it = AsConst(queue).begin();

	it = it++;

	EXPECT_EQ(it, AsConst(queue).begin());
}

TEST(CancelableListTests, dereferenced_const_queue_iterator_has_correct_value)
{
	static constexpr u32 dummyValue = 1234;

	auto queue = CancelableList<CancelableMock>(InlineExecutor());

	queue.Emplace(CancelableMock(dummyValue));

	EXPECT_EQ((*AsConst(queue).begin()).Value, dummyValue);
	EXPECT_EQ(AsConst(queue).begin()->Value, dummyValue);
}

TEST(CancelableListTests, const_iterator_begin_end_is_same_when_empty)
{
	auto queue = CancelableList<CancelableMock>(InlineExecutor());

	EXPECT_EQ(queue.cbegin(), queue.cend());
}

TEST(CancelableListTests, const_iterator_begin_end_is_not_same_when_not_empty)
{
	auto queue = CancelableList<CancelableMock>(InlineExecutor());

	queue.Emplace(CancelableMock(1234));

	EXPECT_NE(queue.cbegin(), queue.cend());
}

TEST(CancelableListTests, const_iterator_begin_end_is_same_when_incremented)
{
	auto queue = CancelableList<CancelableMock>(InlineExecutor());

	queue.Emplace(CancelableMock(1234));

	auto it = queue.cbegin();

	++it;

	EXPECT_EQ(it, queue.cend());
}

TEST(CancelableListTests, increment_const_iterator_is_same_as_begin)
{
	auto queue = CancelableList<CancelableMock>(InlineExecutor());

	queue.Emplace(CancelableMock(1234));

	auto it = queue.cbegin();

	it = it++;

	EXPECT_EQ(it, queue.cbegin());
}

TEST(CancelableListTests, dereferenced_const_iterator_has_correct_value)
{
	static constexpr u32 dummyValue = 1234;

	auto queue = CancelableList<CancelableMock>(InlineExecutor());

	queue.Emplace(CancelableMock(dummyValue));

	EXPECT_EQ((*queue.cbegin()).Value, dummyValue);
	EXPECT_EQ(queue.cbegin()->Value, dummyValue);
}
