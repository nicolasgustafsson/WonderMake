#include "stdafx.h"
#include "DataThreads.h"

#include "ThreadIds.h"
#include "ThreadProcess.h"
#include "ThreadProcessMain.h"

void DataThreads::Start(Program& aProgramReference)
{
	myThreads.resize(3);
	myThreads[static_cast<decltype(myThreads)::size_type>(EThreadId::Render)] = std::make_shared<ThreadProcess>(EThreadId::Render);
	myThreads[static_cast<decltype(myThreads)::size_type>(EThreadId::File)] = std::make_shared<ThreadProcess>(EThreadId::File);
	myThreads[static_cast<decltype(myThreads)::size_type>(EThreadId::Logic)] = std::make_shared<ThreadProcessMain>(aProgramReference);

	myThreads[static_cast<decltype(myThreads)::size_type>(EThreadId::Render)]->Start();
	myThreads[static_cast<decltype(myThreads)::size_type>(EThreadId::File)]->Start();
	myThreads[static_cast<decltype(myThreads)::size_type>(EThreadId::Logic)]->Start(false);
}
