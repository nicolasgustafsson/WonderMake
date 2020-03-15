#include "pch.h"
#include "BuffProperties.h"
#include "Character/CharacterFunctionality.h"

BuffStatChangeBlueprintProperty::BuffStatChangeBlueprintProperty(const SStatChange aStatChange)
	: myStatChange(aStatChange)
{

}

void BuffStatChangeBlueprintProperty::Apply(CharacterFunctionality& aCharacter)
{
	aCharacter.Get<CharacterStatsFunctionality>().ApplyStatChange(myStatChange);
}

void BuffStatChangeBlueprintProperty::Remove(CharacterFunctionality& aCharacter)
{
	aCharacter.Get<CharacterStatsFunctionality>().RemoveStatChange(myStatChange);
}

void BuffStatChangeBlueprintProperty::Inspect() const
{
	myStatChange.Inspect();
}

BuffDamageOverTimeBlueprintProperty::BuffDamageOverTimeBlueprintProperty(const f32 aDamagePerTick, const f32 aTimeBetweenTicks)
	: BuffTimedTickingBlueprintProperty(aTimeBetweenTicks), myDamagePerTick(aDamagePerTick)
{

}

void BuffDamageOverTimeBlueprintProperty::Inspect() const
{
	
}

void BuffDamageOverTimeBlueprintProperty::TimedTick(BuffBlueprintPropertyInstance& aBuffPropertyInstance)
{
	aBuffPropertyInstance.myCharacter.Damage(myDamagePerTick);
}

BuffTimedTickingBlueprintProperty::BuffTimedTickingBlueprintProperty(const f32 aTimeBetweenTicks)
	:myTimeBetweenTicks(aTimeBetweenTicks)
{

}

void BuffTimedTickingBlueprintProperty::ApplyOnBuff(BuffInstance& aBuff)
{
	BuffBlueprintTickPropertyInstance& instance = *reinterpret_cast<BuffBlueprintTickPropertyInstance*>((aBuff.myPropertyInstances.emplace(std::make_unique<BuffBlueprintTickPropertyInstance>(*this, aBuff.myCharacter)))->get());
	instance.myTimeUntilTick = myTimeBetweenTicks;
}

void BuffTimedTickingBlueprintProperty::Tick(BuffBlueprintPropertyInstance& aBuffPropertyInstance)
{
	const f32 deltaTime = SystemPtr<TimeKeeper>()->GetDeltaSeconds();
	BuffBlueprintTickPropertyInstance& instance = *reinterpret_cast<BuffBlueprintTickPropertyInstance*>(&aBuffPropertyInstance);

	instance.myTimeUntilTick -= deltaTime;

	if (instance.myTimeUntilTick < 0.f)
	{
		instance.myTimeUntilTick += myTimeBetweenTicks;
		TimedTick(aBuffPropertyInstance);
	}
}

void BuffBlueprintPropertyInstance::Tick()
{
	myBlueprintProperty.Tick(*this);
}

bool BuffBlueprintPropertyInstance::BuffShouldDie() const
{
	return myBlueprintProperty.BuffShouldDie(*this);
}

void BuffLifetimeProperty::Inspect() const
{

}

void BuffLifetimeProperty::ApplyOnBuff(BuffInstance& aBuff)
{
	BuffLifetimePropertyInstance& instance = *reinterpret_cast<BuffLifetimePropertyInstance*>((aBuff.myPropertyInstances.emplace(std::make_unique<BuffLifetimePropertyInstance>(*this, aBuff.myCharacter, myLifeTime)))->get());
}

void BuffLifetimeProperty::Tick(BuffBlueprintPropertyInstance& aBuffPropertyInstance)
{
	const f32 deltaTime = SystemPtr<TimeKeeper>()->GetDeltaSeconds();
	BuffLifetimePropertyInstance& instance = *reinterpret_cast<BuffLifetimePropertyInstance*>(&aBuffPropertyInstance);

	instance.myLifeLeft -= deltaTime;
}

bool BuffLifetimeProperty::BuffShouldDie(const BuffBlueprintPropertyInstance& aBuffPropertyInstance) const
{
	const BuffLifetimePropertyInstance& instance = *reinterpret_cast<const BuffLifetimePropertyInstance*>(&aBuffPropertyInstance);
	return instance.myLifeLeft < 0.f;
}
