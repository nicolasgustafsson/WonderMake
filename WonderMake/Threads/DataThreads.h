#pragma once

#include "Utilities/Singleton.h"

#include <memory>
#include <vector>
#include <thread>
#include "RoutineIds.h"
#include "Thread.h"
#include "Routine.h"
#include "Utilities/TimeKeeper.h"
#include "System/SystemPtr.h"

class Program;
class Routine;

class DataThreads
	: public Singleton<DataThreads>
{
public:
	DataThreads();

	void Start(Program& aProgramReference, Closure&& aCallback);

	Routine& GetRoutine(const ERoutineId aRoutine);

private:
	std::vector<std::unique_ptr<Routine>> myRoutines;

	std::optional<Thread> myRenderThread;
	std::optional<Thread> myFileThread;

	SystemPtr<TimeKeeper> myTimeKeeper;
};
