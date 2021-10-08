#include "pch.h"

#include "Randomizer.h"

Randomizer::Randomizer()
	: myRandomGenerator(myRandomDevice())
{

}

bool Randomizer::GetRandomBool()
{
	return GetRandomNumber(0, 1) == 1;
}
