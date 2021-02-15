#include "pch.h"
#include "EffectFunctionality.h"

REGISTER_COMPONENT(SEffectComponent);
REGISTER_FUNCTIONALITY(EffectFunctionality);

void EffectFunctionality::Tick()
{
	const f32 aDeltaTime = Get<TimeKeeper>().GetDeltaSeconds();
	for (auto&& effect : Get<SEffectComponent>().Effects)
	{
		effect->Tick(aDeltaTime);
	}

	Get<SEffectComponent>().Effects.EraseIf([](auto&& effect) {return !effect->IsAlive(); });
}
