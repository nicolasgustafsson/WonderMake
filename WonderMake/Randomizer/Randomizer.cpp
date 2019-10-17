#include "pch.h"
#include "Randomizer.h"
#include <random>

Randomizer::Randomizer()
	: myRandomGenerator(myRandomDevice())
{

}

bool Randomizer::GetRandomBool()
{
	return GetRandomNumber(0, 2) == 1;
}
