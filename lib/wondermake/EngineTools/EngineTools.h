#pragma once

#include "Utilities/Debugging/Debugged.h"

#include "wondermake-base/System.h"

class TimeKeeperSingleton;

class EngineTools
	: public System<
		Policy::Set<
			PAdd<TimeKeeperSingleton, PRead>>,
		STrait::Set<
			STGui,
			STSingleton,
			STWonderMake>>
	, public Debugged
{
public:
	inline EngineTools()
		: Debugged("Engine Tools", GetExecutor())
	{}

	virtual void Debug(bool& aIsOpen) override;
};
