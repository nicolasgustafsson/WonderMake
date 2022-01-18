#include "WonderMakeTestsCommon/GTestInclude.h"
#include "WonderMakeTestsCommon/WinPlatformSystemMock.h"

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
	const HANDLE dummyEventHandle = (HANDLE)0x1234ABCD;
	StrictMock<EventCallbackMock> eventCallbackMock;

	auto eventCallback = [&eventCallbackMock]()
	{
		eventCallbackMock.Invoke();
	};

	EXPECT_CALL(myWinPlatformSystem, CreateEventW)
		.WillOnce(Return(dummyInterruptEventHandle));
	EXPECT_CALL(myWinPlatformSystem, SetEvent(dummyInterruptEventHandle))
		.Times(2);

	CreateEventSystem();

	myWinEventSystem->RegisterEvent(dummyEventHandle, eventCallback);
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
	const HANDLE dummyEventHandle = (HANDLE)0x1234ABCD;
	StrictMock<EventCallbackMock> eventCallbackMock;

	auto eventCallback = [&eventCallbackMock]()
	{
		eventCallbackMock.Invoke();
	};

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);
	EXPECT_CALL(myWinPlatformSystem, SetEvent)
		.Times(2);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);
	EXPECT_CALL(myWinPlatformSystem, WaitForMultipleObjects)
		.With(Args<1, 0>(ElementsAre(_, dummyEventHandle)))
		.WillOnce(Return(WAIT_TIMEOUT));

	CreateEventSystem();

	myWinEventSystem->RegisterEvent(dummyEventHandle, eventCallback);

	myWinEventSystem->WaitForEvent(0);
}

TEST_F(WinEventSystemImplTest, callback_is_called_when_event_is_set)
{
	const HANDLE dummyEventHandle = (HANDLE)0x1234ABCD;
	StrictMock<EventCallbackMock> eventCallbackMock;

	auto eventCallback = [&eventCallbackMock]()
	{
		eventCallbackMock.Invoke();
	};

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);
	EXPECT_CALL(myWinPlatformSystem, SetEvent)
		.Times(2);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);
	EXPECT_CALL(myWinPlatformSystem, WaitForMultipleObjects)
		.WillOnce(Return(WAIT_OBJECT_0 + 1));
	EXPECT_CALL(myWinPlatformSystem, WaitForSingleObject(dummyEventHandle, _))
		.WillOnce(Return(WAIT_OBJECT_0));

	EXPECT_CALL(eventCallbackMock, Invoke);

	CreateEventSystem();

	myWinEventSystem->RegisterEvent(dummyEventHandle, eventCallback);

	myWinEventSystem->WaitForEvent(0);
}

TEST_F(WinEventSystemImplTest, unregistered_callback_is_not_called_when_event_is_set)
{
	const HANDLE dummyEventHandle = (HANDLE)0x1234ABCD;
	StrictMock<EventCallbackMock> eventCallbackMock;

	auto eventCallback = [&eventCallbackMock]()
	{
		eventCallbackMock.Invoke();
	};

	EXPECT_CALL(myWinPlatformSystem, CreateEventW);
	EXPECT_CALL(myWinPlatformSystem, SetEvent)
		.Times(3);
	EXPECT_CALL(myWinPlatformSystem, ResetEvent);
	EXPECT_CALL(myWinPlatformSystem, WaitForMultipleObjects)
		.WillOnce(Return(WAIT_OBJECT_0));

	CreateEventSystem();

	myWinEventSystem->RegisterEvent(dummyEventHandle, eventCallback);

	myWinEventSystem->UnregisterEvent(dummyEventHandle);

	myWinEventSystem->WaitForEvent(0);
}
