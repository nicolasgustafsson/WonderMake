#include "wondermake-base/RandomSystem.h"

#include "wondermake-base/SystemGlobal.h"

WM_REGISTER_SYSTEM(RandomSystem);

RandomSystem::RandomSystem()
	: myRandomGenerator(myRandomDevice())
{

}

bool RandomSystem::GetRandomBool()
{
	return GetRandomNumber(0, 1) == 1;
}
