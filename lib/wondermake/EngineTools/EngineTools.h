#pragma once

#include "Utilities/Debugging/Debugged.h"

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
		: Debugged("Engine Tools")
	{}

	virtual void Debug() override;
};
