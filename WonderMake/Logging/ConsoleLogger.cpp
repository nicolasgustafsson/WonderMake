#include "pch.h"
#include "ConsoleLogger.h"
#include <iostream>

REGISTER_SYSTEM(ConsoleLogger);

ConsoleLogger::ConsoleLogger() noexcept
	: mySubscriber(BindHelper(&ConsoleLogger::OnLogMessage, this))
{}

void ConsoleLogger::OnLogMessage(const SLogMessage& aMessage)
{
	if (aMessage.HasTag(TagError))
		SetColor(EConsoleColor::Red);
	else if (aMessage.HasTag(TagWarning))
		SetColor(EConsoleColor::Yellow);
	else if (aMessage.HasTag(TagSuccess))
		SetColor(EConsoleColor::Green);
	else
		SetColor(EConsoleColor::Default);

	std::cout << aMessage.LogText << '\n';
}

void ConsoleLogger::SetColor(const EConsoleColor aColor) noexcept
{
	auto hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	WORD attribute = 0;

	switch (aColor)
	{
	case EConsoleColor::Blue:
		attribute = 9;
		break;
	case EConsoleColor::Green:
		attribute = 10;
		break;
	case EConsoleColor::Yellow:
		attribute = 14;
		break;
	case EConsoleColor::Red:
		attribute = 12;
		break;
	case EConsoleColor::Default:
		attribute = 7;
		break;
	}

	SetConsoleTextAttribute(hConsole, attribute);
}
