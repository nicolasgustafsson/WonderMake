#include "pch.h"
#include "Object.h"
#include "Functionalities/Functionality.h"

Object::~Object()
{
	while (!myFunctionalities.empty())
	{
		myFunctionalities[0].Functionality->Destroy(*this);
		myFunctionalities.erase(myFunctionalities.begin());
	}
}