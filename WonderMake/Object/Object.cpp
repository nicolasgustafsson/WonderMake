#include "stdafx.h"
#include "Object.h"

Object::Object()
{
}

void Object::_AddFunctionality(BaseFunctionality* aFunctionality)
{
	myFunctionalities.push_back(aFunctionality);
}

void Object::_RemoveCapability(BaseFunctionality* aFunctionality)
{
	myFunctionalities.erase(std::find(myFunctionalities.begin(), myFunctionalities.end(), aFunctionality));
}

void Object::_AddComponent(SComponent* aComponent)
{
	myComponents.push_back(aComponent);
}

void Object::_RemoveComponent(SComponent* aComponent)
{
	myComponents.erase(std::find(myComponents.begin(), myComponents.end(), aComponent));
}
