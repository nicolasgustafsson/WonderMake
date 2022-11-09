#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-utility/TimerManager.h"

 // Putting this here so that we know that it compiles with the default time point.
using DefaultTimeManager = TimerManager<std::chrono::high_resolution_clock::time_point>;

class TimerManagerTest
	: public ::testing::Test
{
public:
	inline TimerManagerTest()
		: myTimerManager(InlineExecutor())
	{}

protected:
	TimerManager<u32> myTimerManager;

};

TEST_F(TimerManagerTest, calling_addtimer_returns_uncompleted_valid_future)
{
	auto future = myTimerManager.AddTimer(0);

	EXPECT_TRUE(future.IsValid());
	EXPECT_FALSE(future.IsDone());
}

TEST_F(TimerManagerTest, calling_runexpiredtimers_does_not_complete_non_expired_timer_futures)
{
	auto future = myTimerManager.AddTimer(1);

	myTimerManager.RunExpiredTimers(0);

	EXPECT_FALSE(future.IsDone());
}

TEST_F(TimerManagerTest, calling_runexpiredtimers_completes_expired_timer_futures)
{
	auto futureExpiredA = myTimerManager.AddTimer(0);
	auto futureExpiredB = myTimerManager.AddTimer(1);
	auto futureNonExpired = myTimerManager.AddTimer(2);

	myTimerManager.RunExpiredTimers(1);

	EXPECT_TRUE(futureExpiredA.IsCompleted());
	EXPECT_TRUE(futureExpiredB.IsCompleted());
	EXPECT_FALSE(futureNonExpired.IsDone());
}

TEST_F(TimerManagerTest, calling_runexpiredtimers_completes_expired_timer_futures_when_a_timer_is_cancelled)
{
	auto futureExpired = myTimerManager.AddTimer(0);
	auto futureCancelled = myTimerManager.AddTimer(1);
	auto futureNonExpired = myTimerManager.AddTimer(2);

	bool isCancelledFutureExecuted = false;

	futureCancelled.ThenRun(InlineExecutor(), [&isCancelledFutureExecuted](auto&&) { isCancelledFutureExecuted = true; });

	futureCancelled.Reset();

	myTimerManager.RunExpiredTimers(1);

	EXPECT_TRUE(futureExpired.IsCompleted());
	EXPECT_FALSE(isCancelledFutureExecuted);
	EXPECT_FALSE(futureNonExpired.IsDone());
}

TEST_F(TimerManagerTest, calling_runexpiredtimers_completes_expired_timer_futures_after_previous_call)
{
	auto future = myTimerManager.AddTimer(1);

	myTimerManager.RunExpiredTimers(0);

	myTimerManager.RunExpiredTimers(1);

	EXPECT_TRUE(future.IsCompleted());
}
