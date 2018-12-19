#include "stdafx.h"
#include "ConsoleLogger.h"
#include <iostream>


ConsoleLogger::ConsoleLogger()
	:mySubscriber(ERoutineId::Logic, BindHelper(&ConsoleLogger::OnLogMessage, this))
{

}


ConsoleLogger::~ConsoleLogger()
{
}

void ConsoleLogger::OnLogMessage(const SLogMessage& aMessage)
{
	std::cout << aMessage.LogText << '\n';
}
