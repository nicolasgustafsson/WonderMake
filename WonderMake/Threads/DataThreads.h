#pragma once

#include "Utilities/Singleton.h"

#include <memory>
#include <vector>

class ThreadProcess;

class DataThreads :
	public Singleton<DataThreads>
{
public:
	DataThreads();

private:
	std::vector<std::shared_ptr<ThreadProcess>> myThreads;
};
