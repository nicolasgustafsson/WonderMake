#pragma once

#include "Utilities/Singleton.h"

#include <memory>
#include <vector>

class Program;
class ThreadProcess;

class DataThreads :
	public Singleton<DataThreads>
{
public:
	void Start(Program& aProgramReference);

private:
	std::vector<std::shared_ptr<ThreadProcess>> myThreads;
};
