
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
	CancelableMock(const CancelableMock&) = default;
	CancelableMock(CancelableMock&&) = default;

	CancelableMock& operator=(const CancelableMock&) = default;
	CancelableMock& operator=(CancelableMock&&) = default;

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

TEST(CancelableListTests, insert_by_const_ref_inserts_at_iterator)
{
	static constexpr u32 dummyValueA = 1234;
	static constexpr u32 dummyValueB = 2345;
	static constexpr u32 dummyValueC = 3456;

	const auto mockA = CancelableMock(dummyValueA);
	const auto mockB = CancelableMock(dummyValueB);
	const auto mockC = CancelableMock(dummyValueC);

	auto list = CancelableList<CancelableMock>(InlineExecutor());

	list.insert(list.end(), mockA);

	auto it = list.begin();

	EXPECT_EQ(it->Value, dummyValueA);

	++it;

	EXPECT_EQ(it, list.end());

	list.insert(list.end(), mockB);

	it = list.begin();

	EXPECT_EQ(it->Value, dummyValueA);

	++it;

	EXPECT_EQ(it->Value, dummyValueB);

	++it;

	EXPECT_EQ(it, list.end());

	list.insert(++list.begin(), mockC);

	it = list.begin();

	EXPECT_EQ(it->Value, dummyValueA);

	++it;

	EXPECT_EQ(it->Value, dummyValueC);

	++it;

	EXPECT_EQ(it->Value, dummyValueB);

	++it;

	EXPECT_EQ(it, list.end());
}

TEST(CancelableListTests, insert_by_r_ref_inserts_at_iterator)
{
	static constexpr u32 dummyValueA = 1234;
	static constexpr u32 dummyValueB = 2345;
	static constexpr u32 dummyValueC = 3456;

	auto list = CancelableList<CancelableMock>(InlineExecutor());

	list.insert(list.end(), CancelableMock(dummyValueA));

	auto it = list.begin();

	EXPECT_EQ(it->Value, dummyValueA);

	++it;

	EXPECT_EQ(it, list.end());

	list.insert(list.end(), CancelableMock(dummyValueB));

	it = list.begin();

	EXPECT_EQ(it->Value, dummyValueA);

	++it;

	EXPECT_EQ(it->Value, dummyValueB);

	++it;

	EXPECT_EQ(it, list.end());

	list.insert(++list.begin(), CancelableMock(dummyValueC));

	it = list.begin();

	EXPECT_EQ(it->Value, dummyValueA);

	++it;

	EXPECT_EQ(it->Value, dummyValueC);

	++it;

	EXPECT_EQ(it->Value, dummyValueB);

	++it;

	EXPECT_EQ(it, list.end());
}

TEST(CancelableListTests, insert_by_const_ref_returns_iterator_to_inserted_cancelable)
{
	static constexpr u32 dummyValueA = 1234;
	static constexpr u32 dummyValueB = 2345;
	static constexpr u32 dummyValueC = 3456;

	const auto mockA = CancelableMock(dummyValueA);
	const auto mockB = CancelableMock(dummyValueB);
	const auto mockC = CancelableMock(dummyValueC);

	auto list = CancelableList<CancelableMock>(InlineExecutor());

	auto insertIt = list.insert(list.end(), mockA);

	EXPECT_EQ(insertIt->Value, dummyValueA);

	insertIt = list.insert(list.end(), mockB);

	EXPECT_EQ(insertIt->Value, dummyValueB);

	insertIt = list.insert(++list.begin(), mockC);

	EXPECT_EQ(insertIt->Value, dummyValueC);
}

TEST(CancelableListTests, insert_by_r_ref_returns_iterator_to_inserted_cancelable)
{
	static constexpr u32 dummyValueA = 1234;
	static constexpr u32 dummyValueB = 2345;
	static constexpr u32 dummyValueC = 3456;

	auto list = CancelableList<CancelableMock>(InlineExecutor());

	auto insertIt = list.insert(list.end(), CancelableMock(dummyValueA));

	EXPECT_EQ(insertIt->Value, dummyValueA);

	insertIt = list.insert(list.end(), CancelableMock(dummyValueB));

	EXPECT_EQ(insertIt->Value, dummyValueB);

	insertIt = list.insert(++list.begin(), CancelableMock(dummyValueC));

	EXPECT_EQ(insertIt->Value, dummyValueC);
}

TEST(CancelableListTests, erase_removes_cancelable_at_iterator)
{
	static constexpr u32 dummyValueA = 1234;
	static constexpr u32 dummyValueB = 2345;
	static constexpr u32 dummyValueC = 2345;

	auto list = CancelableList<CancelableMock>(InlineExecutor());

	list.Emplace(CancelableMock(dummyValueA));

	list.Emplace(CancelableMock(dummyValueB));

	list.Emplace(CancelableMock(dummyValueC));

	list.erase(++list.begin());
	
	auto it = list.begin();

	EXPECT_EQ(it->Value, dummyValueA);

	++it;

	EXPECT_EQ(it->Value, dummyValueC);

	++it;

	EXPECT_EQ(it, list.end());
}

TEST(CancelableListTests, erase_removes_range_of_cancelables_between_iterators)
{
	static constexpr u32 dummyValueA = 1234;
	static constexpr u32 dummyValueB = 2345;
	static constexpr u32 dummyValueC = 2345;

	auto list = CancelableList<CancelableMock>(InlineExecutor());

	list.Emplace(CancelableMock(dummyValueA));

	list.Emplace(CancelableMock(dummyValueB));
	list.Emplace(CancelableMock(dummyValueB));
	list.Emplace(CancelableMock(dummyValueB));

	list.Emplace(CancelableMock(dummyValueC));

	auto itFirst = ++list.begin();
	auto itLast = ++++++++list.begin();

	list.erase(itFirst, itLast);

	auto it = list.begin();

	EXPECT_EQ(it->Value, dummyValueA);

	++it;

	EXPECT_EQ(it->Value, dummyValueC);

	++it;

	EXPECT_EQ(it, list.end());
}

TEST(CancelableListTests, erase_returns_element_after_last)
{
	static constexpr u32 dummyValueA = 1234;
	static constexpr u32 dummyValueB = 2345;
	static constexpr u32 dummyValueC = 2345;

	auto list = CancelableList<CancelableMock>(InlineExecutor());

	list.Emplace(CancelableMock(dummyValueA));

	list.Emplace(CancelableMock(dummyValueB));

	list.Emplace(CancelableMock(dummyValueC));

	auto it = list.erase(++list.begin());

	EXPECT_EQ(it->Value, dummyValueC);
}

TEST(CancelableListTests, erase_range_returns_element_after_last)
{
	static constexpr u32 dummyValueA = 1234;
	static constexpr u32 dummyValueB = 2345;
	static constexpr u32 dummyValueC = 2345;

	auto list = CancelableList<CancelableMock>(InlineExecutor());

	list.Emplace(CancelableMock(dummyValueA));

	list.Emplace(CancelableMock(dummyValueB));
	list.Emplace(CancelableMock(dummyValueB));
	list.Emplace(CancelableMock(dummyValueB));

	list.Emplace(CancelableMock(dummyValueC));

	auto itFirst = ++list.begin();
	auto itLast = ++++++++list.begin();

	auto it = list.erase(itFirst, itLast);

	EXPECT_EQ(it->Value, dummyValueC);
}
