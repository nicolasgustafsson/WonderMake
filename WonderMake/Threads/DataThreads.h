#pragma once

#include "Utilities/Singleton.h"

#include <memory>
#include <vector>
#include <thread>
#include "RoutineIds.h"

class Program;
class Routine;

class DataThreads :
	public Singleton<DataThreads>
{
public:
	void Start(Program& aProgramReference, Closure&& aCallback);

	std::shared_ptr<Routine> GetRoutine(const ERoutineId aRoutine);

private:
	std::vector<std::shared_ptr<Routine>> myRoutines;
	std::vector<std::thread> myThreads;
};
