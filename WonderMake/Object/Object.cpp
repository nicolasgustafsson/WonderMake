#include "stdafx.h"
#include "Object.h"
#include "Functionality.h"

Object::Object()
{
}

Object::~Object()
{
	while (!myFunctionalities.empty())
	{
		myFunctionalities[0].Functionality->Destroy(this);
		myFunctionalities.erase(myFunctionalities.begin());
	}
}