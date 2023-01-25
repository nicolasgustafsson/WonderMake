#pragma once

#include "wondermake-utility/AnyExecutor.h"
#include "wondermake-utility/EventSubscriber.h"

#include <functional>
#include <string>
#include <vector>

 // TODO: Debugged is not threadsafe, this needs to be fixed.

class Debugged
{
public:
	struct SDebugData
	{
		std::string								Name;
		AnyExecutor								Executor;
		std::function<void()>					TickFunc;
		std::function<void(EventSubscriber)>	SetSubscriberFunc;
	};

	Debugged(std::string aName, AnyExecutor aExecutor);
	virtual ~Debugged() noexcept = default;

	static std::vector<SDebugData> GetAndResetDebugged();

protected:
	virtual void Debug() = 0;

private:
	static std::vector<SDebugData> ourDebugged;

	EventSubscriber myTickSubscriber;

};

