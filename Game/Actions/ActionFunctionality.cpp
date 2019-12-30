#include "pch.h"
#include "ActionFunctionality.h"
#include "Actions/Action.h"
#include "Character/CharacterFunctionality.h"

ActionFunctionality::ActionFunctionality(Object& aOwner)
	: Super(aOwner) 
{
	Get<ImpulseFunctionality>().Subscribe<SDiedImpulse>(*this, [&](auto) 
		{
			EndCurrentAction();
		});
}

bool ActionFunctionality::IsInAction() const
{
	return Get<SActionComponent>().CurrentAction != nullptr;
}

f32 ActionFunctionality::TimeSinceLastAction() const
{
	if (IsInAction())
		return 0.f;

	return SystemPtr<TimeKeeper>()->TimeSince(Get<SActionComponent>().CompletionTime);
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

	currentAction->Tick();

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
	Action*& currentAction = Get<SActionComponent>().CurrentAction;

	currentAction->EndAction();
	currentAction = nullptr;
	Get<SActionComponent>().CompletionTime = SystemPtr<TimeKeeper>()->GetGameTime();
}
