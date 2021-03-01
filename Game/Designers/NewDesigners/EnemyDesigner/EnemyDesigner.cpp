#include "pch.h"

#include "EnemyDesigner.h"

EnemyDesigner::EnemyDesigner()
{
	AddOperation<GenerateEnemyMovesetOperation>();
	AddOperation<GenerateEnemyColorOperation>();
	AddOperation<CreateEnemyOperation>();
}
