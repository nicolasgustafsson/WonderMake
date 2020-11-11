#include "pch.h"
#include "Thread.h"
#include "Routine.h"
#include "Utilities/Platform.h"


Thread::Thread(const std::string& aName)
	: myThread
	([&] { Run(); })
{
	SetName(aName);
}


Thread::~Thread()
{
}

void Thread::SetName(const std::string& aName)
{
	Platform::SetThreadName(myThread, aName.c_str());
}

void Thread::AddRoutine(Routine& aRoutine)
{
	assert(IsReady());
	myRoutines.push_back(aRoutine);
}

void Thread::DoOnce() noexcept
{
	myIsWaiting = false;
}

void Thread::WaitUntilReady() const noexcept
{
	while (!IsReady())
		std::this_thread::sleep_for(std::chrono::microseconds(10));
}

void Thread::WaitWhileReady() const noexcept
{
	while (IsReady())
		std::this_thread::sleep_for(std::chrono::microseconds(10));
}

void Thread::Run()
{
	for(;;)
	{
		WaitWhileReady();

		for (auto&& routine : myRoutines)
			routine.get().Run();

		myIsWaiting = true;
	}
}