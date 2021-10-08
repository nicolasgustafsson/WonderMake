#include "pch.h"
#include "SubobjectFunctionality.h" 

REGISTER_COMPONENT(SSubobjectComponent);
REGISTER_FUNCTIONALITY(SubobjectFunctionality);

void SubobjectFunctionality::Destroy()
{
	Get<SSubobjectComponent>().SlatedForRemoval = true;
}

ObjectContainerFunctionality* SubobjectFunctionality::GetLevel() const
{
	return Get<SSubobjectComponent>().Level;
}
