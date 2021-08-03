#include "pch.h"
#include "AttachedFunctionality.h"

REGISTER_FUNCTIONALITY(AttachedFunctionality)

void AttachedFunctionality::Tick()
{
	if (!myAttachedObject)
		return;

	Get<TransformFunctionality2D>().SetPosition(myAttachedObject->GetPosition());
}

void AttachedFunctionality::SetAttachedObject(TransformFunctionality2D* aObject)
{
	myAttachedObject = aObject;
}
