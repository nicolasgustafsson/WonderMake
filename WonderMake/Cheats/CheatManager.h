#pragma once
class CheatManager : public System<>, Debugged
{
public:
	 CheatManager()
		: Debugged("CheatManager")
	{
	}

	void AddCheat(std::string aCheatName, Closure&& aCheat);
	void RemoveCheat(std::string aCheatName);
 protected:
	 void Debug() override;
 private:
	 Container<Closure, Key<std::string>> myCheats;
};

class Cheat
{
public:
	Cheat(std::string aName, Closure&& aFunction)
	{
		myName = aName;
		SystemPtr<CheatManager>()->AddCheat(myName, std::move(aFunction));
	}

	~Cheat()
	{
		SystemPtr<CheatManager>()->RemoveCheat(myName);
	}

private:
	std::string myName;
};

