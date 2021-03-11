#pragma once
#include "Functionalities/Functionality.h"
#include "Effect.h"
#include "Utilities/Container/Container.h"

struct SEffectComponent : public SComponent
{
	Container<EffectInstance> Effects;
};

class EffectFunctionality
	: public Functionality<Policy::Set<
	PAdd<SEffectComponent, PWrite>,
	PAdd<TimeKeeper, PRead>
	>>
{
public:
	void ApplyEffect(EffectBlueprint&& aBlueprint)
	{
		Get<SEffectComponent>().Effects.Add(EffectInstance(std::move(aBlueprint)));
	}

	void ApplyEffect(std::string_view aBlueprintName)
	{
		SystemPtr<AssetDatabase<EffectBlueprint>>()->GetAsset(aBlueprintName);
	}

	void Tick();
};

