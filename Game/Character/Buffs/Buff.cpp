#include "pch.h"
#include "Buff.h"
#include "Character/CharacterFunctionality.h"

BuffStatChangeProperty::BuffStatChangeProperty(const SStatChange aStatChange)
	: myStatChange(aStatChange)
{

}

void BuffStatChangeProperty::Apply(CharacterFunctionality& aCharacter)
{
	aCharacter.Get<CharacterStatsFunctionality>().ApplyStatChange(myStatChange);
}

void BuffStatChangeProperty::Remove(CharacterFunctionality& aCharacter)
{
	aCharacter.Get<CharacterStatsFunctionality>().RemoveStatChange(myStatChange);
}

void BuffStatChangeProperty::Inspect() const
{
	myStatChange.Inspect();
}

class Buff BuffBlueprint::Instantiate(CharacterFunctionality& aCharacter) const
{
	Buff buff(*this, aCharacter);

	for (auto&& property : myProperties)
	{
		property->Apply(aCharacter);
	}

	return buff;
}

void BuffBlueprint::Inspect()
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
