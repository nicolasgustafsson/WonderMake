#include "wondermake-tests-common/GTestInclude.h"

#include "wondermake-utility/Stopwatch.h"

// Putting this here so that we know that it compiles with the default time point.
using DefaultStopwatch = Stopwatch<std::chrono::high_resolution_clock>;

inline constexpr auto locStartTime = std::chrono::nanoseconds(0);

class ClockImpl
{
public:
	using rep                       = i64;
	using period                    = std::nano;
	using duration                  = std::chrono::nanoseconds;
	using time_point                = std::chrono::time_point<ClockImpl>;
	static constexpr bool is_steady = true;

	inline ClockImpl()
	{
		SetTick(locStartTime);
	}

	inline void SetTick(duration aDuration)
	{
		ON_CALL(*this, now)
			.WillByDefault(Return(time_point(aDuration)));
	}

	MOCK_METHOD(time_point, now, ());
};

class ClockMock
{
public:
	using rep                       = ClockImpl::rep;
	using period                    = ClockImpl::period;
	using duration                  = ClockImpl::duration;
	using time_point                = ClockImpl::time_point;
	static constexpr bool is_steady = ClockImpl::is_steady;

	inline ClockMock(ClockImpl& aClock)
		: Clock(aClock)
	{}

	inline time_point now() const
	{
		return Clock.now();
	}

private:
	ClockImpl& Clock;

};

class StopwatchTest
: public ::testing::Test
{
public:
	StopwatchTest()
		: myStopwatch(myClock)
	{}
	
protected:
	ClockImpl myClock;

	Stopwatch<ClockMock> myStopwatch;

};

TEST_F(StopwatchTest, getelapsedtime_returns_correct_duration)
{
	static constexpr auto dummyTime = ClockMock::duration(1);

	myClock.SetTick(dummyTime);

	const auto duration = myStopwatch.GetElapsedTime<ClockMock::duration>();

	EXPECT_EQ(duration, dummyTime);
}

TEST_F(StopwatchTest, stop_returns_correct_duration)
{
	static constexpr auto dummyTime = ClockMock::duration(1);

	myClock.SetTick(dummyTime);

	const auto duration = myStopwatch.Stop<ClockMock::duration>();

	EXPECT_EQ(duration, dummyTime);
}

TEST_F(StopwatchTest, reset_returns_correct_duration)
{
	static constexpr auto dummyTime = ClockMock::duration(1);

	myClock.SetTick(dummyTime);

	const auto duration = myStopwatch.Restart<ClockMock::duration>();

	EXPECT_EQ(duration, dummyTime);
}

TEST_F(StopwatchTest, restart_returns_correct_duration)
{
	static constexpr auto dummyTime = ClockMock::duration(1);

	myClock.SetTick(dummyTime);

	const auto duration = myStopwatch.Restart<ClockMock::duration>();

	EXPECT_EQ(duration, dummyTime);
}

TEST_F(StopwatchTest, calling_start_when_stopwatch_is_running_does_nothing)
{
	static constexpr auto dummyTimeStart	= ClockMock::duration(1);
	static constexpr auto dummyTimeEnd		= ClockMock::duration(3);

	static constexpr auto dummyTime			= dummyTimeEnd;

	myClock.SetTick(dummyTimeStart);

	myStopwatch.Start();

	myClock.SetTick(dummyTimeEnd);

	const auto duration = myStopwatch.GetElapsedTime<ClockMock::duration>();

	EXPECT_EQ(duration, dummyTime);
}

TEST_F(StopwatchTest, getelapsedtime_returns_zero_duration_when_stopwatch_is_reset)
{
	static constexpr auto dummyTimeStop		= ClockMock::duration(1);
	static constexpr auto dummyTimeEnd		= ClockMock::duration(3);
	static constexpr auto dummyTime			= ClockMock::duration(0);

	myClock.SetTick(dummyTimeStop);

	myStopwatch.Reset();

	myClock.SetTick(dummyTimeEnd);

	const auto duration = myStopwatch.GetElapsedTime<ClockMock::duration>();

	EXPECT_EQ(duration, dummyTime);
}

TEST_F(StopwatchTest, stop_returns_zero_duration_when_stopwatch_is_reset)
{
	static constexpr auto dummyTimeStop		= ClockMock::duration(1);
	static constexpr auto dummyTimeEnd		= ClockMock::duration(3);
	static constexpr auto dummyTime			= ClockMock::duration(0);

	myClock.SetTick(dummyTimeStop);

	myStopwatch.Reset();

	myClock.SetTick(dummyTimeEnd);

	const auto duration = myStopwatch.Stop<ClockMock::duration>();

	EXPECT_EQ(duration, dummyTime);
}

TEST_F(StopwatchTest, reset_returns_zero_duration_when_stopwatch_is_reset)
{
	static constexpr auto dummyTimeStop		= ClockMock::duration(1);
	static constexpr auto dummyTimeEnd		= ClockMock::duration(3);
	static constexpr auto dummyTime			= ClockMock::duration(0);

	myClock.SetTick(dummyTimeStop);

	myStopwatch.Reset();

	myClock.SetTick(dummyTimeEnd);

	const auto duration = myStopwatch.Reset<ClockMock::duration>();

	EXPECT_EQ(duration, dummyTime);
}

TEST_F(StopwatchTest, restart_returns_zero_duration_when_stopwatch_is_reset)
{
	static constexpr auto dummyTimeStop		= ClockMock::duration(1);
	static constexpr auto dummyTimeEnd		= ClockMock::duration(3);
	static constexpr auto dummyTime			= ClockMock::duration(0);

	myClock.SetTick(dummyTimeStop);

	myStopwatch.Reset();

	myClock.SetTick(dummyTimeEnd);

	const auto duration = myStopwatch.Restart<ClockMock::duration>();

	EXPECT_EQ(duration, dummyTime);
}

TEST_F(StopwatchTest, getelapsedtime_returns_correct_duration_when_stopwatch_is_restarted)
{
	static constexpr auto dummyTimeStart	= ClockMock::duration(1);
	static constexpr auto dummyTimeEnd		= ClockMock::duration(3);

	static constexpr auto dummyTime			= dummyTimeEnd - dummyTimeStart;

	myClock.SetTick(dummyTimeStart);

	myStopwatch.Restart();

	myClock.SetTick(dummyTimeEnd);

	const auto duration = myStopwatch.GetElapsedTime<ClockMock::duration>();

	EXPECT_EQ(duration, dummyTime);
}

TEST_F(StopwatchTest, stop_returns_correct_duration_when_stopwatch_is_restarted)
{
	static constexpr auto dummyTimeStart	= ClockMock::duration(1);
	static constexpr auto dummyTimeEnd		= ClockMock::duration(3);

	static constexpr auto dummyTime			= dummyTimeEnd - dummyTimeStart;

	myClock.SetTick(dummyTimeStart);

	myStopwatch.Restart();

	myClock.SetTick(dummyTimeEnd);

	const auto duration = myStopwatch.Stop<ClockMock::duration>();

	EXPECT_EQ(duration, dummyTime);
}

TEST_F(StopwatchTest, reset_returns_correct_duration_when_stopwatch_is_restarted)
{
	static constexpr auto dummyTimeStart	= ClockMock::duration(1);
	static constexpr auto dummyTimeEnd		= ClockMock::duration(3);

	static constexpr auto dummyTime			= dummyTimeEnd - dummyTimeStart;

	myClock.SetTick(dummyTimeStart);

	myStopwatch.Restart();

	myClock.SetTick(dummyTimeEnd);

	const auto duration = myStopwatch.Reset<ClockMock::duration>();

	EXPECT_EQ(duration, dummyTime);
}

TEST_F(StopwatchTest, restart_returns_correct_duration_when_stopwatch_is_restarted)
{
	static constexpr auto dummyTimeStart	= ClockMock::duration(1);
	static constexpr auto dummyTimeEnd		= ClockMock::duration(3);

	static constexpr auto dummyTime			= dummyTimeEnd - dummyTimeStart;

	myClock.SetTick(dummyTimeStart);

	myStopwatch.Restart();

	myClock.SetTick(dummyTimeEnd);

	const auto duration = myStopwatch.Restart<ClockMock::duration>();

	EXPECT_EQ(duration, dummyTime);
}

TEST_F(StopwatchTest, calling_stop_when_stopwatch_is_stopped_does_nothing)
{
	static constexpr auto dummyTimeStop		= ClockMock::duration(1);
	static constexpr auto dummyTimeEnd		= ClockMock::duration(3);

	static constexpr auto dummyTime			= dummyTimeStop;

	myClock.SetTick(dummyTimeStop);

	myStopwatch.Stop();

	myClock.SetTick(dummyTimeEnd);

	const auto duration = myStopwatch.GetElapsedTime<ClockMock::duration>();

	EXPECT_EQ(duration, dummyTime);
}

TEST_F(StopwatchTest, getelapsedtime_returns_correct_duration_when_stopwatch_is_stopped)
{
	static constexpr auto dummyTimeStart	= ClockMock::duration(1);
	static constexpr auto dummyTimeStop		= ClockMock::duration(3);
	static constexpr auto dummyTimeAfter	= ClockMock::duration(6);

	static constexpr auto dummyTime			= dummyTimeStop - dummyTimeStart;

	myClock.SetTick(dummyTimeStart);

	myStopwatch.Restart();

	myClock.SetTick(dummyTimeStop);

	myStopwatch.Stop();

	myClock.SetTick(dummyTimeAfter);

	const auto duration = myStopwatch.GetElapsedTime<ClockMock::duration>();

	EXPECT_EQ(duration, dummyTime);
}

TEST_F(StopwatchTest, stop_returns_correct_duration_when_stopwatch_is_stopped)
{
	static constexpr auto dummyTimeStart	= ClockMock::duration(1);
	static constexpr auto dummyTimeStop		= ClockMock::duration(3);
	static constexpr auto dummyTimeAfter	= ClockMock::duration(6);

	static constexpr auto dummyTime			= dummyTimeStop - dummyTimeStart;

	myClock.SetTick(dummyTimeStart);

	myStopwatch.Restart();

	myClock.SetTick(dummyTimeStop);

	myStopwatch.Stop();

	myClock.SetTick(dummyTimeAfter);

	const auto duration = myStopwatch.Stop<ClockMock::duration>();

	EXPECT_EQ(duration, dummyTime);
}

TEST_F(StopwatchTest, reset_returns_correct_duration_when_stopwatch_is_stopped)
{
	static constexpr auto dummyTimeStart	= ClockMock::duration(1);
	static constexpr auto dummyTimeStop		= ClockMock::duration(3);
	static constexpr auto dummyTimeAfter	= ClockMock::duration(6);

	static constexpr auto dummyTime			= dummyTimeStop - dummyTimeStart;

	myClock.SetTick(dummyTimeStart);

	myStopwatch.Restart();

	myClock.SetTick(dummyTimeStop);

	myStopwatch.Stop();

	myClock.SetTick(dummyTimeAfter);

	const auto duration = myStopwatch.Reset<ClockMock::duration>();

	EXPECT_EQ(duration, dummyTime);
}

TEST_F(StopwatchTest, restart_returns_correct_duration_when_stopwatch_is_stopped)
{
	static constexpr auto dummyTimeStart	= ClockMock::duration(1);
	static constexpr auto dummyTimeStop		= ClockMock::duration(3);
	static constexpr auto dummyTimeAfter	= ClockMock::duration(6);

	static constexpr auto dummyTime			= dummyTimeStop - dummyTimeStart;

	myClock.SetTick(dummyTimeStart);

	myStopwatch.Restart();

	myClock.SetTick(dummyTimeStop);

	myStopwatch.Stop();

	myClock.SetTick(dummyTimeAfter);

	const auto duration = myStopwatch.Restart<ClockMock::duration>();

	EXPECT_EQ(duration, dummyTime);
}

TEST_F(StopwatchTest, getelapsedtime_returns_correct_duration_after_stopped_stopwatch_is_resumed)
{
	static constexpr auto dummyTimeStop		= ClockMock::duration(3);
	static constexpr auto dummyTimeStart	= ClockMock::duration(6);
	static constexpr auto dummyTimeEnd		= ClockMock::duration(13);

	static constexpr auto dummyTime			= dummyTimeStop + dummyTimeEnd - dummyTimeStart;

	myClock.SetTick(dummyTimeStop);

	myStopwatch.Stop();

	myClock.SetTick(dummyTimeStart);

	myStopwatch.Start();

	myClock.SetTick(dummyTimeEnd);

	const auto duration = myStopwatch.GetElapsedTime<ClockMock::duration>();

	EXPECT_EQ(duration, dummyTime);
}

TEST_F(StopwatchTest, stop_returns_correct_duration_after_stopped_stopwatch_is_resumed)
{
	static constexpr auto dummyTimeStop		= ClockMock::duration(3);
	static constexpr auto dummyTimeStart	= ClockMock::duration(6);
	static constexpr auto dummyTimeEnd		= ClockMock::duration(13);

	static constexpr auto dummyTime			= dummyTimeStop + dummyTimeEnd - dummyTimeStart;

	myClock.SetTick(dummyTimeStop);

	myStopwatch.Stop();

	myClock.SetTick(dummyTimeStart);

	myStopwatch.Start();

	myClock.SetTick(dummyTimeEnd);

	const auto duration = myStopwatch.Stop<ClockMock::duration>();

	EXPECT_EQ(duration, dummyTime);
}

TEST_F(StopwatchTest, reset_returns_correct_duration_after_stopped_stopwatch_is_resumed)
{
	static constexpr auto dummyTimeStop		= ClockMock::duration(3);
	static constexpr auto dummyTimeStart	= ClockMock::duration(6);
	static constexpr auto dummyTimeEnd		= ClockMock::duration(13);

	static constexpr auto dummyTime			= dummyTimeStop + dummyTimeEnd - dummyTimeStart;

	myClock.SetTick(dummyTimeStop);

	myStopwatch.Stop();

	myClock.SetTick(dummyTimeStart);

	myStopwatch.Start();

	myClock.SetTick(dummyTimeEnd);

	const auto duration = myStopwatch.Reset<ClockMock::duration>();

	EXPECT_EQ(duration, dummyTime);
}

TEST_F(StopwatchTest, restart_returns_correct_duration_after_stopped_stopwatch_is_resumed)
{
	static constexpr auto dummyTimeStop		= ClockMock::duration(3);
	static constexpr auto dummyTimeStart	= ClockMock::duration(6);
	static constexpr auto dummyTimeEnd		= ClockMock::duration(13);

	static constexpr auto dummyTime			= dummyTimeStop + dummyTimeEnd - dummyTimeStart;

	myClock.SetTick(dummyTimeStop);

	myStopwatch.Stop();

	myClock.SetTick(dummyTimeStart);

	myStopwatch.Start();

	myClock.SetTick(dummyTimeEnd);

	const auto duration = myStopwatch.Restart<ClockMock::duration>();

	EXPECT_EQ(duration, dummyTime);
}
