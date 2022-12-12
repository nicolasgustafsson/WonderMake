#pragma once

#include "Utilities/Debugging/Debugged.h"

class TimeKeeper;

class EngineTools
	: public System<
		Policy::Set<
			PAdd<TimeKeeper, PRead>>,
		STrait::Set<
			STGui,
			STWonderMake>>
	, public Debugged
{
public:
	inline EngineTools()
		: Debugged("Engine Tools")
	{}

	virtual void Debug() override;
};
