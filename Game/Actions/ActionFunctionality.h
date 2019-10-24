#pragma once
#include <any>
#include "Actions/Action.h"

class Action;

struct SActionComponent
	: public SComponent
{
	std::any CurrentActionValue;
	Action* CurrentAction = nullptr;

	f32 CompletionTime = -999.f;
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
		if (IsInAction())
		{
			Action* currentAction = Get<SActionComponent>().CurrentAction;

			if (!currentAction->CanBeInterrupted())
				return ActionResult::Failed;

			EndCurrentAction();
		}

		Get<SActionComponent>().CurrentActionValue = aAction;

		std::any_cast<TAction&>(Get<SActionComponent>().CurrentActionValue).BeginAction();
		Get<SActionComponent>().CurrentAction = &std::any_cast<TAction&>(Get<SActionComponent>().CurrentActionValue);

		return ActionResult::Succeeded;
	}

	template<typename TAction>
	bool WasLastActionOfType() const
	{
		return typeid(TAction) == Get<SActionComponent>().CurrentActionValue.type();
	}

	bool IsInAction() const;

	f32 TimeSinceLastAction() const;

	Action* GetCurrentAction() const;
	
	void Tick();

	void Inspect();

private:	
	void EndCurrentAction();
};

