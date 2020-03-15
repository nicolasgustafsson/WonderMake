#include "pch.h"
#include "BuffLifetimeProperty.h"
#include "Character/Buffs/Buff.h"

void BuffLifetimeProperty::ApplyOnBuff(BuffInstance& aBuff)
{
	aBuff.myPropertyInstances.emplace(std::make_unique<BuffLifetimePropertyInstance>(*this, aBuff.myCharacter, myLifeTime));
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

void BuffLifetimeProperty::Inspect() const
{
	ImGui::Text("Lifetime = %f", myLifeTime);
}

void BuffLifetimeProperty::InspectInstance(BuffBlueprintPropertyInstance& aBuffPropertyInstance) const
{
	const BuffLifetimePropertyInstance& instance = *reinterpret_cast<const BuffLifetimePropertyInstance*>(&aBuffPropertyInstance);

	std::string timeLeftTickString = ("Time left = ") + std::to_string(instance.myLifeLeft) + "/" + std::to_string(myLifeTime);

	ImGui::ProgressBar((instance.myLifeLeft) / (myLifeTime), ImVec2(-1.f, 0.f), timeLeftTickString.c_str());
}
