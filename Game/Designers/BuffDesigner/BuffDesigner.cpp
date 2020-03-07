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
	const f32 strength = ((aBuffDesign.NumericIntensity * aHowMuch) / 100.f) + 1.0f;
	std::unique_ptr<BuffStatChangeProperty> statChangeProperty = std::make_unique<BuffStatChangeProperty>(SStatChange{ ECharacterStat::MovementSpeed, strength });
	aBuffDesign.Properties.insert(std::move(statChangeProperty));
}

void BuffDesigner::MakeBuffWorse(const f32 aHowMuch, SBuffDesign& aBuffDesign) const
{
	const f32 strength = (1.0f / ((aBuffDesign.NumericIntensity * aHowMuch) / 100.f + 1.0f));
	std::unique_ptr<BuffStatChangeProperty> statChangeProperty = std::make_unique<BuffStatChangeProperty>(SStatChange{ ECharacterStat::MovementSpeed, strength });
	aBuffDesign.Properties.insert(std::move(statChangeProperty));
}

BuffBlueprint BuffDesigner::ConstructBlueprintFromDesign(SBuffDesign& aBuffDesign) const
{
	BuffBlueprint blueprint;

	blueprint.myDuration = aBuffDesign.Duration;
	blueprint.myProperties = std::move(aBuffDesign.Properties);

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