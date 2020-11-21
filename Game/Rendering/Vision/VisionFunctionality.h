#pragma once
#include "UtilityFunctionalities/LineOfSightFunctionality.h"
#include "Rendering/Vision/VisionRenderObject.h"

struct SVisionComponent
	: public SComponent
{
	VisionRenderObject VisionRenderObject;
};


class VisionFunctionality : public Functionality<
	Policy::Set<
	PAdd<LineOfSightFunctionality, PRead>,
	PAdd<TransformFunctionality, PRead>,
	PAdd<SVisionComponent, PWrite>>>
{
public:
	void Tick();
};

