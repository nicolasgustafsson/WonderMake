#include "pch.h"
#include "EngineTools.h"
#include "EasingTester.h"

WM_REGISTER_SYSTEM(EngineTools);

void EngineTools::Debug()
{
	EasingTester::TestEasings();
}
