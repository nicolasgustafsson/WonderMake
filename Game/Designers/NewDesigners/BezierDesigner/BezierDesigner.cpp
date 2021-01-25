#include "pch.h"
#include "BezierDesigner.h"
#include "BezierOperations.h"

BezierDesigner::BezierDesigner()
{
	AddOperation<AddMissingBezierAttributes>();
	AddOperation<CreateControlPoints>();
	AddOperation<DesignBezier>();
}
