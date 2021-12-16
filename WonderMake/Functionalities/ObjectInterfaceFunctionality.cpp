#include "pch.h"
#include "ObjectInterfaceFunctionality.h"

REGISTER_FUNCTIONALITY(ObjectInterfaceFunctionality)

void ObjectInterfaceFunctionality::OnNewObject(BaseObject& aObjectAddedTo)
{
	myObject = &aObjectAddedTo;
}
