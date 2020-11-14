#include "pch.h"
#include "ActionFunctionality.h"
#include "Actions/Action.h"
#include "Character/CharacterFunctionality.h"

REGISTER_COMPONENT(SActionComponent);
REGISTER_FUNCTIONALITY(ActionFunctionality);

bool ActionFunctionality::IsInAction() const
{
	return Get<SActionComponent>().CurrentAction != nullptr;
}

f32 ActionFunctionality::TimeSinceLastAction() const
{
	if (IsInAction())
		return 0.f;

	return Get<TimeKeeper>().TimeSince(Get<SActionComponent>().CompletionTime);
}

Action* ActionFunctionality::GetCurrentAction() const
{
	return Get<SActionComponent>().CurrentAction;
}

void ActionFunctionality::Tick()
{
	Action* currentAction = Get<SActionComponent>().CurrentAction;
	if (!currentAction)
		return;

	currentAction->Tick(Get<TimeKeeper>().GetDeltaSeconds());

	if (currentAction->IsCompleted())
		EndCurrentAction();
}

void ActionFunctionality::Inspect()
{
	Action* currentAction = Get<SActionComponent>().CurrentAction;
	if (currentAction)
		currentAction->Inspect();
}

void ActionFunctionality::EndCurrentAction()
{
	if (!IsInAction())
		return;

	Action*& currentAction = Get<SActionComponent>().CurrentAction;

	currentAction->EndAction();
	currentAction = nullptr;
	Get<SActionComponent>().CompletionTime = Get<TimeKeeper>().GetGameTime();
}
