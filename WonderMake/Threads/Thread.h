#pragma once
#include <vector>

class Routine;

class Thread
{
public:
	Thread(const std::string& aName);
	~Thread();

	void SetName(const std::string& aName);

	void AddRoutine(Routine& aRoutine);

	void DoOnce() noexcept;

	void WaitUntilReady() const noexcept;
	inline bool IsReady() const noexcept { return myIsWaiting; };

private:
	void Run();

	std::vector<std::reference_wrapper<Routine>> myRoutines;
	std::thread myThread;
	std::atomic_bool myIsWaiting = true;
};

