#pragma once
#include <any>
#include "Actions/Action.h"

class Action;

struct SActionComponent
	: public SComponent
{
	std::any myCurrentActionValue;
	Action* myCurrentAction = nullptr;
};


enum class ActionResult
{
	Succeeded,
	Failed,
	//Buffered?
};

//Nicos: This currently is not thread safe, have an extra look when adding policies/threading
class ActionFunctionality
	: public Functionality<ActionFunctionality, SActionComponent>
{
public:
	ActionFunctionality(Object& aOwner);

	template<typename TAction> //requires std::is_base_of_v<Action, TAction> uncomment this when concepts is a thing
	ActionResult StartAction(TAction aAction)
	{
		Action* currentAction = Get<SActionComponent>().myCurrentAction;

		if (currentAction && !currentAction->CanBeInterrupted())
			return ActionResult::Failed;

		Get<SActionComponent>().myCurrentActionValue = aAction;

		std::any_cast<TAction&>(Get<SActionComponent>().myCurrentActionValue).BeginAction();
		Get<SActionComponent>().myCurrentAction = &std::any_cast<TAction&>(Get<SActionComponent>().myCurrentActionValue);

		return ActionResult::Succeeded;
	}

	Action* GetCurrentAction() const;
	
	void Tick();

	void Inspect();
};

