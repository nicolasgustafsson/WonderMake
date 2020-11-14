#pragma once
#include <any>
#include "Actions/Action.h"

class Action;
class TimeKeeper;

struct SActionComponent
	: public SComponent
{
	std::any CurrentActionValue;
	Action* CurrentAction = nullptr;

	f32 CompletionTime = -999.f;
};

enum class EActionResult
{
	Succeeded,
	Failed,
	//Buffered?
};

//Nicos: This currently is not thread safe, have an extra look when adding policies/threading
class ActionFunctionality
	: public Functionality<
		Policy::Set<
			PAdd<TimeKeeper, PRead>,
			PAdd<SActionComponent, PWrite>>>
{
public:
	template<typename TAction> requires std::is_base_of_v<Action, TAction>
	inline EActionResult StartAction(TAction aAction)
	{
		if (IsInAction())
		{
			Action* currentAction = Get<SActionComponent>().CurrentAction;

			if (!currentAction->CanBeInterrupted())
				return EActionResult::Failed;

			EndCurrentAction();
		}

		Get<SActionComponent>().CurrentActionValue = aAction;

		TAction& newAction = std::any_cast<TAction&>(Get<SActionComponent>().CurrentActionValue);
		newAction.BeginAction();

		Get<SActionComponent>().CurrentAction = &newAction;

		return EActionResult::Succeeded;
	}

	template<typename TAction>
	inline bool WasLastActionOfType() const noexcept
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
