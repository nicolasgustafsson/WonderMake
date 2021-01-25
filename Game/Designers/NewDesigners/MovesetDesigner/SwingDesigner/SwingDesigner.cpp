#include "pch.h"
#include "SwingDesigner.h"
#include "SwingOperations/SwingOperations.h"

SwingDesigner::SwingDesigner()
{
	//boilerplate - we need to add every operation :(
	AddOperation<GenerateSwingPath>();
	AddOperation<FinalizeSwing>();
}
