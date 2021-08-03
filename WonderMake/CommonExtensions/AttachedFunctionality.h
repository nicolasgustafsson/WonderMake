#pragma once
#include "Functionalities/TransformFunctionality.h"
class AttachedFunctionality : public Functionality<TransformFunctionality2D>
{
public:
	void Tick();

	void SetAttachedObject(TransformFunctionality2D* aObject);

private:

	TransformFunctionality2D* myAttachedObject = nullptr;
};

