#pragma once

#include "Utilities/Singleton.h"

#include <memory>
#include <vector>
#include <thread>

class Program;
class Routine;

class DataThreads :
	public Singleton<DataThreads>
{
public:
	void Start(Program& aProgramReference);

private:
	std::vector<std::shared_ptr<Routine>> myRoutines;
	std::vector<std::thread> myThreads;
};
