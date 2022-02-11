#pragma once

#include "System.h"

#include "wondermake-utility/CmdLineArgs.h"

class CmdLineArgsSystem
	: public System<>
	, public CmdLineArgs
{
public:
	CmdLineArgsSystem(CmdLineArgs aCmdLineArgs);

};
