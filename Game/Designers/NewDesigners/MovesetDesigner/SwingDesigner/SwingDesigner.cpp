#include "pch.h"
#include "SwingDesigner.h"
#include "SwingOperations/SwingOperations.h"

SwingDesigner::SwingDesigner()
{
	//boilerplate - we need to add every operation :(
	AddOperation<RandomizeSwingSpeed>(); 
	AddOperation<DetermineSwingTimings>();
	AddOperation<GenerateSwingPath>();
	AddOperation<DetermineSwingDamage>();
	AddOperation<FinalizeSwing>();
}
