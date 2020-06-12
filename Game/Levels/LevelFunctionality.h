#pragma once
#include "Functionalities/Functionality.h"

class LevelDenizenFunctionality;

struct SLevelComponent : public SComponent
{
	struct SDenizen
	{
		SDenizen(Object&& aDenizen, LevelDenizenFunctionality& aDenizenFunctionality)
			: DenizenObject(std::move(aDenizen)/*WHY IS THIS MOVE NEEDED I DONT GET IT AAAAAAA*/), DenizenFunctionality(aDenizenFunctionality)
		{

		}
		Object DenizenObject;
		LevelDenizenFunctionality& DenizenFunctionality;
	};

	plf::colony<SDenizen> Denizens;

	SVector2f StartPosition;
};

class LevelFunctionality : public Functionality<
	LevelFunctionality,
	Policy::Set<Policy::Add<SLevelComponent, Policy::EPermission::Write>>>
{
public:
	LevelFunctionality(Object& aOwner);

	Object& AddDenizen();
	Object& AddDenizen(Object&& aObject);

	void AddDenizens(plf::colony<Object>&& aObjects);

	void SetStartPosition(const SVector2f aPosition);
	[[nodiscard]] SVector2f GetStartPosition() const noexcept { return Get<SLevelComponent>().StartPosition; }
};

