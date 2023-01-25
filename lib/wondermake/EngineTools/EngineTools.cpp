#include "pch.h"

#include "EngineTools.h"
#include "EasingTester.h"

#include "Utilities/TimeKeeper.h"

#include "wondermake-base/SystemGlobal.h"

WM_REGISTER_SYSTEM(EngineTools);

void EngineTools::Debug()
{
	EasingTester::TestEasings(Get<TimeKeeperSingleton>().GetDeltaTime());
}
