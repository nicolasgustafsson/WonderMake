#include "stdafx.h"
#include "Object.h"

Object::Object()
{
}

void Object::_AddCapability(BaseCapability* aCapability)
{
	myCapabilities.push_back(aCapability);
}

void Object::_RemoveCapability(BaseCapability* aCapability)
{
	myCapabilities.erase(std::find(myCapabilities.begin(), myCapabilities.end(), aCapability));
}

void Object::_AddComponent(SComponent* aComponent)
{
	myComponents.push_back(aComponent);
}

void Object::_RemoveComponent(SComponent* aComponent)
{
	myComponents.erase(std::find(myComponents.begin(), myComponents.end(), aComponent));
}
