#include "stdafx.h"
#include "DataThreads.h"

#include "ThreadIds.h"
#include "ThreadProcess.h"

DataThreads::DataThreads()
{
	myThreads.resize(3);
	myThreads[static_cast<decltype(myThreads)::size_type>(EThreadId::Logic)] = std::make_shared<ThreadProcess>(EThreadId::Logic);
	myThreads[static_cast<decltype(myThreads)::size_type>(EThreadId::Render)] = std::make_shared<ThreadProcess>(EThreadId::Render);
	myThreads[static_cast<decltype(myThreads)::size_type>(EThreadId::File)] = std::make_shared<ThreadProcess>(EThreadId::File);
}
