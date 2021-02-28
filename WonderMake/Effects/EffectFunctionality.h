#pragma once
#include "Functionalities/Functionality.h"
#include "Effect.h"
#include "Utilities/Container/Container.h"

struct SEffectComponent : public SComponent
{
	Container<Effect> Effects;
};

class EffectFunctionality
	: public Functionality<Policy::Set<
	PAdd<SEffectComponent, PWrite>,
	PAdd<TimeKeeper, PRead>
	>>
{
public:
	void ApplyEffect(std::string_view AssetLink)
	{
		Get<SEffectComponent>().Effects.Add(Effect());
	}
	void Tick();
};

