#include "pch.h"

#include "Randomizer.h"

REGISTER_SYSTEM(Randomizer);

Randomizer::Randomizer(Dependencies&& aDependencies)
	: Super(std::move(aDependencies))
	, myRandomGenerator(myRandomDevice())
{

}

bool Randomizer::GetRandomBool()
{
	return GetRandomNumber(0, 1) == 1;
}
