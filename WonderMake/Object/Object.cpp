#include "pch.h"
#include "Object.h"
#include "Functionalities/Functionality.h"

Object::~Object()
{
	while (!myFunctionalities.empty())
	{
		myFunctionalities[myFunctionalities.size() - 1].Functionality->Destroy(*this);
		myFunctionalities.erase(myFunctionalities.end() - 1);
	}
}