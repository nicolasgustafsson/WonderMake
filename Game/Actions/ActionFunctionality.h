#pragma once
class Action;

struct SActionComponent
	: public SComponent
{
	Action* myCurrentAction = nullptr;
};

class ActionFunctionality
	: public Functionality<ActionFunctionality, SActionComponent>
{
public:
	ActionFunctionality(Object& aOwner);

	void StartAction(Action& aAction);
	
	void Tick();

	void Inspect();
};

