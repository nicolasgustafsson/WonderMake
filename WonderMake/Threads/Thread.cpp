#include "stdafx.h"
#include "Thread.h"
#include "Routine.h"
#include "ThreadNaming.h"


Thread::Thread(const std::string& aName)
	:myThread
	([&] {
	while (true)
	{
		while (myIsWaiting)
			std::this_thread::sleep_for(std::chrono::microseconds(100));

		for (auto&& routine : myRoutines)
			routine.get().Run();

		myIsWaiting = true;
	}
	})
{
	SetName(aName);
}


Thread::~Thread()
{
}

void Thread::SetName(const std::string& aName)
{
	ThreadNaming::SetThreadName(myThread, aName.c_str());
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
		std::this_thread::sleep_for(std::chrono::microseconds(100));
}
