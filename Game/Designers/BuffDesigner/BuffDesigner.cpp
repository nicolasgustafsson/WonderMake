#include "pch.h"
#include "BuffDesigner.h"
#include "Randomizer/Randomizer.h"

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

	myLastBuffDesign = std::move(design);

	return *myBlueprint.insert(std::move(blueprint)); 
}

EBuffType BuffDesigner::DecideBuffType() const
{
	return SystemPtr<Randomizer>()->GetRandomBool() ? EBuffType::Debuff : EBuffType::Buff;
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
	AddStatProperty(true, aHowMuch, aBuffDesign);
}

void BuffDesigner::MakeBuffWorse(const f32 aHowMuch, SBuffDesign& aBuffDesign) const
{
	switch (SystemPtr<Randomizer>()->GetRandomNumber<i32>(0, 1))
	{
	case 0:
		AddStatProperty(false, aHowMuch, aBuffDesign);
		break;
	case 1:
		AddDamageOverTimeProperty(aHowMuch, aBuffDesign);
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
	std::unique_ptr<BuffStatChangeBlueprintProperty> statChangeProperty = std::make_unique<BuffStatChangeBlueprintProperty>(SStatChange{ statToDecrease, strength });

	aBuffDesign.Properties.insert(std::move(statChangeProperty));
}

void BuffDesigner::AddDamageOverTimeProperty(const f32 aDotStrength, SBuffDesign& aBuffDesign) const
{
	const f32 damage = aDotStrength;

	const f32 ticks = 10.f;

	const f32 tickTime = (aBuffDesign.Duration - 0.1f) / ticks;

	const f32 damagePerTick = damage / ticks;

	std::unique_ptr<BuffDamageOverTimeBlueprintProperty> damageProperty = std::make_unique<BuffDamageOverTimeBlueprintProperty>(damagePerTick, tickTime);
	aBuffDesign.Properties.insert(std::move(damageProperty));
}

BuffBlueprint BuffDesigner::ConstructBlueprintFromDesign(SBuffDesign& aBuffDesign) const
{
	BuffBlueprint blueprint;

	blueprint.myDuration = aBuffDesign.Duration;
	blueprint.myProperties = std::move(aBuffDesign.Properties);
	blueprint.myBuffType = aBuffDesign.Type;

	return blueprint;
}

void BuffDesigner::Debug()
{
	ImGui::Begin("Buff Designer");

	if (ImGui::Button("Design Buff"))
	{
		myDebugBuffBlueprint = &DesignBuff({});
	}

	if (myLastBuffDesign.Type == EBuffType::Buff)
		ImGui::Text("Positive buff!");
	else
		ImGui::Text("Debuff!");

	ImGui::Text("Strength: %f", myLastBuffDesign.Strength);
	ImGui::Text("Intensity: %f", myLastBuffDesign.NumericIntensity);
	ImGui::Text("Duration: %f", myLastBuffDesign.Duration);

	if(myDebugBuffBlueprint)
		myDebugBuffBlueprint->Inspect();

	ImGui::End();
}