#pragma once
#include "Functionalities/Functionality.h"
#include "Effect.h"
#include "Utilities/Container/Container.h"

struct SEffectComponent : public SComponent
{
	Container<std::unique_ptr<Effect>> Effects;
};

class EffectFunctionality
	: public Functionality<Policy::Set<
	PAdd<SEffectComponent, PWrite>,
	PAdd<TimeKeeper, PRead>
	>>
{
public:
	template<typename TEffect>
	void ApplyEffect()
	{
		Get<SEffectComponent>().Effects.Add(std::make_unique<TEffect>());
	}
	void Tick();
};

