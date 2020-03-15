#include "pch.h"
#include "BuffTimedTickProperty.h"
#include "Character/Buffs/Buff.h"
#include "Character/CharacterFunctionality.h"

BuffTimedTickProperty::BuffTimedTickProperty(const f32 aTimeBetweenTicks)
	:myTimeBetweenTicks(aTimeBetweenTicks)
{

}

void BuffTimedTickProperty::ApplyOnBuff(BuffInstance& aBuff)
{
	BuffTimedTickPropertyInstance& instance = *reinterpret_cast<BuffTimedTickPropertyInstance*>((aBuff.myPropertyInstances.emplace(std::make_unique<BuffTimedTickPropertyInstance>(*this, aBuff.myCharacter)))->get());
	instance.myTimeUntilTick = myTimeBetweenTicks;
}

void BuffTimedTickProperty::Tick(BuffBlueprintPropertyInstance& aBuffPropertyInstance)
{
	const f32 deltaTime = SystemPtr<TimeKeeper>()->GetDeltaSeconds();
	BuffTimedTickPropertyInstance& instance = *reinterpret_cast<BuffTimedTickPropertyInstance*>(&aBuffPropertyInstance);

	instance.myTimeUntilTick -= deltaTime;

	if (instance.myTimeUntilTick < 0.f)
	{
		instance.myTimeUntilTick += myTimeBetweenTicks;
		TimedTick(aBuffPropertyInstance);
	}
}

void BuffTimedTickProperty::Inspect() const
{
	ImGui::Text("Time between ticks = %f", myTimeBetweenTicks);
}

void BuffTimedTickProperty::InspectInstance(BuffBlueprintPropertyInstance& aBuffPropertyInstance) const
{
	BuffTimedTickPropertyInstance& instance = *reinterpret_cast<BuffTimedTickPropertyInstance*>(&aBuffPropertyInstance);

	std::string timeUntilTickString = ("Time ") + std::to_string(instance.myTimeUntilTick) + "/" + std::to_string(myTimeBetweenTicks);

	ImGui::ProgressBar((instance.myTimeUntilTick) / (myTimeBetweenTicks), ImVec2(-1.f, 0.f), timeUntilTickString.c_str());
}

void BuffDamageOverTimeProperty::TimedTick(BuffBlueprintPropertyInstance& aBuffPropertyInstance)
{
	aBuffPropertyInstance.myCharacter.Damage(myDamagePerTick);
}

void BuffDamageOverTimeProperty::Inspect() const
{
	BuffTimedTickProperty::Inspect();

	ImGui::Text("Damage on tick = %f", myDamagePerTick);
}
