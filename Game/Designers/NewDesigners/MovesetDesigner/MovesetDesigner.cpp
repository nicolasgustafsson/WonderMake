#include "pch.h"
#include "MovesetDesigner.h"
#include "MovesetOperations/MovesetOperations.h"

MovesetDesigner::MovesetDesigner()
{	
	AddOperation<AddMove>();
	AddOperation<SmashTogetherMoveset>();
}
