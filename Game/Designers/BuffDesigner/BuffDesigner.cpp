#include "pch.h"
#include "BuffDesigner.h"
#include "Randomizer/Randomizer.h"
#include "Character/Buffs/LifetimeBuffProperty/BuffLifetimeProperty.h"
#include "Character/Buffs/TimedTickBuffProperty/BuffTimedTickProperty.h"
#include "Character/Buffs/StatChangeProperty/BuffStatChangeProperty.h"
#include "Designers/EffectDesigner/EffectDesigner.h"
#include "Character/Effects/CharacterEffect.h"

BuffDesigner::BuffDesigner() 
	: Debugged("Buff Designer")
{

}

BuffBlueprint& BuffDesigner::DesignBuff(SBuffRequirements aBuffRequirements)
{
	SBuffDesign design;

	design.Type = aBuffRequirements.Type.value_or(DecideBuffType());
	design.Strength = aBuffRequirements.Strength.value_or(DecideBuffStrength());
	design.NumericIntensity = aBuffRequirements.Intensity.value_or(DecideBuffIntensity());

	design.Duration = 10.f / design.NumericIntensity;

	switch (design.Type)
	{
	case EBuffType::Buff:
		MakeBuffBetter(design.Strength, design);
		break;

	case EBuffType::Debuff:
		MakeBuffWorse(design.Strength, design);
		break;
	}

	std::unique_ptr<BuffLifetimeProperty> lifeTimeProperty = std::make_unique<BuffLifetimeProperty>(design.Duration);

	design.Properties.insert(std::move(lifeTimeProperty));

	BuffBlueprint blueprint = ConstructBlueprintFromDesign(design);

	return *myBlueprints.insert(std::move(blueprint));
}

EBuffType BuffDesigner::DecideBuffType() const
{
	return SystemPtr<Randomizer>()->SelectOne<EBuffType, EBuffType::Debuff, EBuffType::Buff>();
}

f32 BuffDesigner::DecideBuffStrength() const
{
	return SystemPtr<Randomizer>()->GetRandomNumber(50.f, 150.f);
}

f32 BuffDesigner::DecideBuffIntensity() const
{
	return std::powf(SystemPtr<Randomizer>()->GetRandomNumber(1.f, 1000.f) / 1000.f, 2.f) * 10.f;
}

void BuffDesigner::MakeBuffBetter(const f32 aHowMuch, SBuffDesign& aBuffDesign) const
{
	switch (SystemPtr<Randomizer>()->GetRandomNumber<i32>(0, 1))
	{
	case 0:
		AddStatProperty(true, aHowMuch, aBuffDesign);
		break;
	case 1:
		AddEffectOverTimeProperty(aHowMuch, aBuffDesign);
		break;
	}
}

void BuffDesigner::MakeBuffWorse(const f32 aHowMuch, SBuffDesign& aBuffDesign) const
{
	switch (SystemPtr<Randomizer>()->GetRandomNumber<i32>(0, 1))
	{
	case 0:
		AddStatProperty(false, aHowMuch, aBuffDesign);
		break;
	case 1:
		AddEffectOverTimeProperty(aHowMuch, aBuffDesign);
		break;
	}
}

void BuffDesigner::AddStatProperty(bool aIncrease, const f32 aStrength, SBuffDesign& aBuffDesign) const
{
	f32 strength = (aBuffDesign.NumericIntensity * aStrength) / 100.f + 1.0f;

	//[Nicos]: Invert
	if (!aIncrease)
		strength = 1.0f / strength;

	ECharacterStat statToDecrease = static_cast<ECharacterStat>(SystemPtr<Randomizer>()->GetRandomNumber<i32>(0, static_cast<i32>(ECharacterStat::Count) - 1));
	std::unique_ptr<BuffStatChangeProperty> statChangeProperty = std::make_unique<BuffStatChangeProperty>(SStatChange{ statToDecrease, strength });

	aBuffDesign.Properties.insert(std::move(statChangeProperty));
}

void BuffDesigner::AddEffectOverTimeProperty(const f32 aEffectStrength, SBuffDesign& aBuffDesign) const
{
	const f32 ticks = 10.f;

	const f32 tickTime = (aBuffDesign.Duration - 0.1f) / ticks;

	SEffectRequirements effectRequirements;
	effectRequirements.Strength = aEffectStrength / ticks;
	effectRequirements.Type = aBuffDesign.Type == EBuffType::Buff ? EEffectType::Positive : EEffectType::Negative;
	std::unique_ptr<BuffEffectOverTimeProperty> characterEffect = std::make_unique<BuffEffectOverTimeProperty>(tickTime, SystemPtr<EffectDesigner>()->DesignCharacterEffect(effectRequirements));
	aBuffDesign.Properties.insert(std::move(characterEffect));
}

BuffBlueprint BuffDesigner::ConstructBlueprintFromDesign(SBuffDesign& aBuffDesign) const
{
	BuffBlueprint blueprint;

	blueprint.myProperties = std::move(aBuffDesign.Properties);
	blueprint.myBuffType = aBuffDesign.Type;

	return blueprint;
}

void BuffDesigner::Debug()
{
	ImGui::ShowDemoWindow();
	ImGui::Begin("Buff Designer");

	for (auto& blueprint : myBlueprints)
	{
		ImGui::PushID(&blueprint);
		if (ImGui::TreeNode("Blueprint"))
		{
			blueprint.Inspect();
			ImGui::TreePop();
		}
		ImGui::PopID();
	}

	ImGui::End();
}