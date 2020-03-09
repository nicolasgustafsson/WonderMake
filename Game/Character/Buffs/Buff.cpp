#include "pch.h"
#include "Buff.h"

BuffInstance BuffBlueprint::Instantiate(CharacterFunctionality& aCharacter) const
{
	BuffInstance buff(*this, aCharacter);

	for (auto&& property : myProperties)
	{
		property->Apply(aCharacter);
		property->ApplyOnBuff(buff);
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

void BuffInstance::Inspect()
{
	myBlueprint.Inspect();

	ImGui::ProgressBar(GetEstimatedPercentLeft());
}

f32 BuffInstance::GetEstimatedPercentLeft() const
{
	return myTimeLeft / myBlueprint.myDuration;
}

void BuffInstance::Tick()
{
	for (auto& propertyInstance : myPropertyInstances)
		propertyInstance->Tick();
}
