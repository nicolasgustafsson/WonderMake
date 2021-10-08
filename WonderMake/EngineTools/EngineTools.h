#pragma once
#include "Utilities/Debugging/Debugged.h"

class EngineTools : public Systemus<EngineTools>, public Debugged
{
public:
	EngineTools()
		: Debugged("Engine Tools")
	{}

	virtual void Debug() override;
};
