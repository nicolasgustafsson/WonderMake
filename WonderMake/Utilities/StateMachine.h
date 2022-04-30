#pragma once

template<typename TEnumState> requires std::is_enum_v<TEnumState>
class StateMachine
{
public:
	StateMachine()
	{
		SwitchState(TEnumState{});
	}

	void UpdateStateMachine()
	{
		TEnumState newState = EvaluateState();

		if (myCurrentState != newState)
		{
			SwitchState(newState);
		}
	}


	virtual TEnumState EvaluateState() const = 0;

	virtual void OnNewState(const TEnumState aNewState) {}

	f32 GetTimeInState() const
	{
		SystemPtr<Wm::TimeKeeper> timeKeeper;
		return timeKeeper->TimeSince(myStateSwitchTime);
	}

	TEnumState GetCurrentState() const
	{
		return myCurrentState;
	}

private:
	void SwitchState(const TEnumState aNewState)
	{
		SystemPtr<Wm::TimeKeeper> timeKeeper;

		myCurrentState = aNewState;
		myStateSwitchTime = timeKeeper->GetGameTime();

		OnNewState(myCurrentState);
	}
	
	f32 myStateSwitchTime = 0.f;
	TEnumState myCurrentState;
};

