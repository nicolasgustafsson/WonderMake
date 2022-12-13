#pragma once

#include "System.h"

#include "wondermake-utility/CmdLineArgs.h"

class CmdLineArgsSystem
	: public System<
		Policy::Set<>,
		STrait::Set<
			STSingleton,
			STWonderMake>>
	, public CmdLineArgs
{
public:
	CmdLineArgsSystem(CmdLineArgs aCmdLineArgs);

};
