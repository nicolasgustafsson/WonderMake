#include "pch.h"
#include "ActionFunctionality.h"
#include "Actions/Action.h"
#include <TimeKeeper.h>

ActionFunctionality::ActionFunctionality(Object& aOwner)
	: Super(aOwner) {}

f32 ActionFunctionality::TimeSinceLastAction() const
{
	return SystemPtr<TimeKeeper>()->TimeSince(Get<SActionComponent>().CompletionTime);
}

Action* ActionFunctionality::GetCurrentAction() const
{
	return Get<SActionComponent>().CurrentAction;
}

void ActionFunctionality::Tick()
{
	Action*& currentAction = Get<SActionComponent>().CurrentAction;
	if (!currentAction)
		return;

	currentAction->Tick();

	if (currentAction->IsCompleted())
	{
		currentAction->EndAction();
		currentAction = nullptr;
		Get<SActionComponent>().CompletionTime = SystemPtr<TimeKeeper>()->GetGameTime();
	}
}

void ActionFunctionality::Inspect()
{
	Action* currentAction = Get<SActionComponent>().CurrentAction;
	if (currentAction)
		currentAction->Inspect();
}
