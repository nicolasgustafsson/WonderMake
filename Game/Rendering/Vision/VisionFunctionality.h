#pragma once
#include "UtilityFunctionalities/LineOfSightFunctionality.h"
#include "Rendering/Vision/VisionRenderObject.h"

struct SVisionComponent
	: public SComponent
{
	VisionRenderObject VisionRenderObject;
};

REGISTER_COMPONENT(SVisionComponent);

class VisionFunctionality : public Functionality<
	VisionFunctionality,
	Policy::Set<
	Policy::Add<LineOfSightFunctionality, Policy::EPermission::Read>,
	Policy::Add<TransformFunctionality, Policy::EPermission::Read>,
	Policy::Add<SVisionComponent, Policy::EPermission::Write>
	>>
{
public:
	VisionFunctionality(Object& aOwner) : Super(aOwner) {}
	void Tick();
};

REGISTER_FUNCTIONALITY(VisionFunctionality);
