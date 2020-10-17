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
	for (auto& property : myProperties)
	{
		property->Inspect();
		ImGui::Separator();
	}
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
	for (auto& propertyInstance : myPropertyInstances)
		propertyInstance->Inspect();

	//ImGui::ProgressBar(GetEstimatedPercentLeft());
}

//f32 BuffInstance::GetEstimatedPercentLeft() const
//{
//	return myTimeLeft / myBlueprint.myDuration;
//}

void BuffInstance::Tick(const f32 aDeltaTime)
{
	for (auto& propertyInstance : myPropertyInstances)
		propertyInstance->Tick(aDeltaTime);
}

bool BuffInstance::ShouldDie() const
{
	for (auto& propertyInstance : myPropertyInstances)
	{
		if (propertyInstance->BuffShouldDie())
			return true;
	}

	return false;
}
