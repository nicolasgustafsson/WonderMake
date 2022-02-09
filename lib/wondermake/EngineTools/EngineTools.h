#pragma once
#include "Utilities/Debugging/Debugged.h"

class EngineTools
	: public System<
		Policy::Set<>,
		STrait::Set<
			STGui>>
	, public Debugged
{
public:
	EngineTools()
		: Debugged("Engine Tools")
	{}

	virtual void Debug() override;
};