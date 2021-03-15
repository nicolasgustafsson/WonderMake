#pragma once

#include "Components/Component.h"
#include "Functionalities/Functionality.h"
#include "Movesets/Moveset.h"
#include "Weapons/WeaponSwing.h"

#include "Utilities/Vector.h"

#include <vector>

class ActionFunctionality;
class CharacterFunctionality;

struct SMovesetComponent
	: public SComponent
{
	SMoveset Moveset;
	i32 CurrentSwingIndex = 0;
};

class MovesetFunctionality
	: public Functionality<
		Policy::Set<
			Policy::Add<ActionFunctionality, PWrite>,
			Policy::Add<CharacterFunctionality, PWrite>,
			Policy::Add<SMovesetComponent, PWrite>>>
{
public:
	void SetMoveset(const SMoveset& aMoveset);

	void Perform(SVector2f aDirection);
};
