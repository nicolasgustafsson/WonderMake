#include "pch.h"
#include "MovesetDesigner.h"
#include "MovesetOperations/MovesetOperations.h"

MovesetDesigner::MovesetDesigner()
{	
	AddOperation<DetermineAmountOfMoves>();
	AddOperation<AddMoves>();
	AddOperation<SmashTogetherMoveset>();
}
