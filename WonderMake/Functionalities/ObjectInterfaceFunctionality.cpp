#include "pch.h"
#include "ObjectInterfaceFunctionality.h"

REGISTER_FUNCTIONALITY(ObjectInterfaceFunctionality)

void ObjectInterfaceFunctionality::OnNewObject(Object& aObjectAddedTo)
{
	myObject = &aObjectAddedTo;
}
