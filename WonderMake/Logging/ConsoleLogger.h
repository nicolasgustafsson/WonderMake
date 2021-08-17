#pragma once
#include "Message/MessageTypes.h"
#include "Message/MessageSubscriber.h"

#include "System/System.h"

enum class EConsoleColor
{
	Red,
	Blue,
	Green,
	Yellow,
	Default
};

class ConsoleLogger
	: public System<>
{
public:
	ConsoleLogger() noexcept;

private:
	void OnLogMessage(const SLogMessage& aMessage);
	void SetColor(const EConsoleColor aColor) noexcept;

	MessageSubscriber mySubscriber;
};

