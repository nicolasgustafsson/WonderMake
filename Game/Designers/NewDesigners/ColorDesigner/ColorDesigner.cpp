#include "pch.h"

#include "ColorDesigner.h"
#include "ColorOperations.h"

ColorDesigner::ColorDesigner()
{
	AddOperation<MovesetToColorOperation>();
	AddOperation<RandomColorOperation>();
}
