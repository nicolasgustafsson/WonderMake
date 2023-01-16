#include "wondermake-tests-common/GTestInclude.h"

#include "WonderMakeBaseTests/WinPlatformSystemMock.h"

#include "WinEventSystemImpl.h"

#include <functional>

class EventCallbackMock
{
public:
	MOCK_METHOD(void, Invoke, (), ());

};

class WinEventSystemImplTest
	: public ::testing::Test
{
protected:
	void CreateEventSystem()
	{
		WinEventSystemImpl::InjectDependencies(std::tie(myWinPlatformSystem));

		myWinEventSystem = std::make_shared<WinEventSystemImpl>();
	}

	StrictMock<WinPlatformSystemMock> myWinPlatformSystem;
	std::shared_ptr<WinEventSystemImpl> myWinEventSystem;

};

TEST_F(WinEventSystemImplTest, creates_and_sets_event_when_constructed_and_destroyed)
{
	const HANDLE dummyInterruptEventHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateEventW(NULL, TRUE, FALSE, NULL))
		.WillOnce(Return(dummyInterruptEventHandle));
	EXPECT_CALL(myWinPlatformSystem, SetEvent(dummyInterruptEventHandle));

	CreateEventSystem();
}

TEST_F(WinEventSystemImplTest, calls_setevent_when_registering_an_event)
{
	const HANDLE dummyInterruptEventHandle = (HANDLE)0x1234ABCD;

	const WinEventHandle dummyEventHandle(dummyInterruptEventHandle, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.WillOnce(Return(dummyInterruptEventHandle));
	EXPECT_CALL(myWinPlatformSystem, SetEvent(dummyInterruptEventHandle))
		.Times(3);

	CreateEventSystem();

	{
		const auto future = myWinEventSystem->RegisterEvent(dummyEventHandle);

		ASSERT_TRUE(future.IsValid());
		ASSERT_FALSE(future.IsCompleted());
		ASSERT_FALSE(future.IsCanceled());
	}

	EXPECT_CALL(myWinPlatformSystem, CloseHandle);
}

TEST_F(WinEventSystemImplTest, interrupt_event_is_passed_to_waitformultipleobjects)
{
	const HANDLE dummyInterruptEventHandle = (HANDLE)0x1234ABCD;

	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.WillOnce(Return(dummyInterruptEventHandle));
	EXPECT_CALL(myWinPlatformSystem, SetEvent);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent(dummyInterruptEventHandle));
	EXPECT_CALL(myWinPlatformSystem, WaitForMultipleObjects)
		.With(Args<1, 0>(ElementsAre(dummyInterruptEventHandle)))
		.WillOnce(Return(WAIT_TIMEOUT));

	CreateEventSystem();

	myWinEventSystem->WaitForEvent(0);
}

TEST_F(WinEventSystemImplTest, callback_event_is_passed_to_waitformultipleobjects)
{
	const WinEventHandle dummyEventHandle((HANDLE)0x1234ABCD, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);
	EXPECT_CALL(myWinPlatformSystem, SetEvent)
		.Times(3);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);
	EXPECT_CALL(myWinPlatformSystem, WaitForMultipleObjects)
		.With(Args<1, 0>(ElementsAre(_, dummyEventHandle.Get())))
		.WillOnce(Return(WAIT_TIMEOUT));

	CreateEventSystem();

	const auto future = myWinEventSystem->RegisterEvent(dummyEventHandle);

	myWinEventSystem->WaitForEvent(0);

	ASSERT_TRUE(future.IsValid());
	ASSERT_FALSE(future.IsCompleted());
	ASSERT_FALSE(future.IsCanceled());

	EXPECT_CALL(myWinPlatformSystem, CloseHandle);
}

TEST_F(WinEventSystemImplTest, callback_is_called_when_event_is_set)
{
	const WinEventHandle dummyEventHandle((HANDLE)0x1234ABCD, myWinPlatformSystem);

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);
	EXPECT_CALL(myWinPlatformSystem, SetEvent)
		.Times(2);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);
	EXPECT_CALL(myWinPlatformSystem, WaitForMultipleObjects)
		.WillOnce(Return(WAIT_OBJECT_0 + 1));
	EXPECT_CALL(myWinPlatformSystem, WaitForSingleObject(dummyEventHandle.Get(), _))
		.WillOnce(Return(WAIT_OBJECT_0));

	CreateEventSystem();

	const auto future = myWinEventSystem->RegisterEvent(dummyEventHandle);

	myWinEventSystem->WaitForEvent(0);

	ASSERT_TRUE(future.IsValid());
	ASSERT_TRUE(future.IsCompleted());
	ASSERT_FALSE(future.IsCanceled());

	EXPECT_CALL(myWinPlatformSystem, CloseHandle);
}

TEST_F(WinEventSystemImplTest, canceled_event_is_not_called_when_event_is_set)
{
	const WinEventHandle dummyEventHandle((HANDLE)0x1234ABCD, myWinPlatformSystem);
	
	EXPECT_CALL(myWinPlatformSystem, CreateEventW);
	EXPECT_CALL(myWinPlatformSystem, SetEvent)
		.Times(3);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);
	EXPECT_CALL(myWinPlatformSystem, WaitForMultipleObjects)
		.WillOnce(Return(WAIT_OBJECT_0));

	CreateEventSystem();

	(void)myWinEventSystem->RegisterEvent(dummyEventHandle);

	myWinEventSystem->WaitForEvent(0);

	EXPECT_CALL(myWinPlatformSystem, CloseHandle);
}
