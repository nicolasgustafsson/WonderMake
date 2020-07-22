#pragma once
#include "Functionalities/Functionality.h"

#include "Physics/Navmesh/NavmeshFunctionality.h"

class LevelDenizenFunctionality;

//[Nicos]: This kind of Level-Leveldenizen structure can probably be generalized into a more generic architecture of using some kind of object container functionality in the future.
//[Nicos]: The basic idea is just to be able to access the level to spawn and remove objects whenever

struct SLevelComponent : public SComponent
{
	struct SDenizen
	{
		SDenizen(Object&& aDenizen, LevelDenizenFunctionality& aDenizenFunctionality)
			: DenizenObject(std::move(aDenizen)), DenizenFunctionality(aDenizenFunctionality)
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
	Policy::Set<
	Policy::Add<SLevelComponent, Policy::EPermission::Write>,
	Policy::Add<NavmeshFunctionality, Policy::EPermission::Write>>>
{
public:
	LevelFunctionality(Object& aOwner);

	void Tick();

	Object& AddDenizen();
	Object& AddDenizen(Object&& aObject);

	void RemoveDenizen(Object& aObject);

	void AddDenizens(plf::colony<Object>&& aObjects);

	void TransferToNewLevel(LevelFunctionality& aNewLevel);

	void SetStartPosition(const SVector2f aPosition);
	[[nodiscard]] SVector2f GetStartPosition() const noexcept { return Get<SLevelComponent>().StartPosition; }
};

