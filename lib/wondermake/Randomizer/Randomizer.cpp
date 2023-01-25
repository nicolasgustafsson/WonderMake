#include "pch.h"

#include "Randomizer.h"

#include "wondermake-base/SystemGlobal.h"

WM_REGISTER_SYSTEM(Randomizer);

Randomizer::Randomizer()
	: myRandomGenerator(myRandomDevice())
{

}

bool Randomizer::GetRandomBool()
{
	return GetRandomNumber(0, 1) == 1;
}
