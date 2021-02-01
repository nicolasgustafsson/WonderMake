#include "pch.h"
#include "MovesetDesigner.h"
#include "MovesetOperations/MovesetOperations.h"

MovesetDesigner::MovesetDesigner()
{	
	AddOperation<RandomizeMovesetType>();
	AddOperation<DetermineAmountOfMoves>();
	AddOperation<AddMoves>();
	AddOperation<SmashTogetherMoveset>();
}
