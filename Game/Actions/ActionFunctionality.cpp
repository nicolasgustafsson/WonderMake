#include "pch.h"
#include "ActionFunctionality.h"
#include "Actions/Action.h"

ActionFunctionality::ActionFunctionality(Object& aOwner)
	: Super(aOwner) {}

Action* ActionFunctionality::GetCurrentAction() const
{
	return Get<SActionComponent>().myCurrentAction;
}

void ActionFunctionality::Tick()
{
	Action*& currentAction = Get<SActionComponent>().myCurrentAction;
	if (!currentAction)
		return;

	currentAction->Tick();

	if (currentAction->IsCompleted())
	{
		currentAction->EndAction();
		currentAction = nullptr;
	}
}

void ActionFunctionality::Inspect()
{
	Action* currentAction = Get<SActionComponent>().myCurrentAction;
	if (currentAction)
		currentAction->Inspect();
}
