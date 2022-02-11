#include "wondermake-base/CmdLineArgsSystem.h"

CmdLineArgsSystem::CmdLineArgsSystem(CmdLineArgs aCmdLineArgs)
	: CmdLineArgs(std::move(aCmdLineArgs))
{}
