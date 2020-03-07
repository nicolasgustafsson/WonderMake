#include "pch.h"
#include "Buff.h"

Buff BuffBlueprint::Instantiate(CharacterFunctionality& aCharacter) const
{
	Buff buff(*this, aCharacter);

	for (auto&& property : myProperties)
	{
		property->Apply(aCharacter);
	}

	return buff;
}

void BuffBlueprint::Inspect() const
{
	ImGui::Separator();
	
	for (auto& property : myProperties)
	{
		property->Inspect();
	}

	ImGui::Text("%fs Duration", myDuration);

	ImGui::Separator();
}

void BuffBlueprint::RemoveFrom(CharacterFunctionality& aCharacter) const
{
	for (auto&& property : myProperties)
	{
		property->Remove(aCharacter);
	}
}

void Buff::Inspect()
{
	myBlueprint.Inspect();

	ImGui::ProgressBar(GetEstimatedPercentLeft());
}

f32 Buff::GetEstimatedPercentLeft() const
{
	return myTimeLeft / myBlueprint.myDuration;
}