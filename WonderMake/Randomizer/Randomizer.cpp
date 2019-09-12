#include "pch.h"
#include "Randomizer.h"
#include <random>

Randomizer::Randomizer()
	: myRandomGenerator(myRandomDevice())
{

}
