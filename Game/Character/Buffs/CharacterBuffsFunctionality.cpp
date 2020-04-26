#include "pch.h"
#include "CharacterBuffsFunctionality.h"
#include "Character/CharacterFunctionality.h"

CharacterBuffsFunctionality::CharacterBuffsFunctionality(Object& aOwner)
	: Super(aOwner)
{
	Get<ImpulseFunctionality>().Subscribe<SDiedImpulse>(*this, [&](auto) 
		{
			ClearBuffs();
		});
}

void CharacterBuffsFunctionality::ApplyBuff(class CharacterFunctionality& aCharacter, BuffBlueprint& aBlueprint)
{
	SCharacterBuffComponent& buffComponent = Get<SCharacterBuffComponent>();

	buffComponent.Buffs.insert(aBlueprint.Instantiate(aCharacter));
}

void CharacterBuffsFunctionality::ClearBuffs()
{
	auto& buffs = Get<SCharacterBuffComponent>().Buffs;
	for (auto& buff : buffs)
	{
		buff.myBlueprint.RemoveFrom(buff.myCharacter);
	}

	buffs.clear();
}

bool CharacterBuffsFunctionality::HasBuff(BuffBlueprint& aBuffBlueprint) const
{
	auto& buffs = Get<SCharacterBuffComponent>().Buffs;

	return std::find_if(buffs.cbegin(), buffs.cend(), [&aBuffBlueprint](const BuffInstance& aBuffInstance) {return &aBuffInstance.myBlueprint == &aBuffBlueprint; }) != buffs.cend();
}

void CharacterBuffsFunctionality::Tick()
{
	const f32 deltaTime = SystemPtr<TimeKeeper>()->GetDeltaSeconds();
	SCharacterBuffComponent& buffComponent = Get<SCharacterBuffComponent>();

	auto it = buffComponent.Buffs.begin();
	while (it != buffComponent.Buffs.end())
	{
		auto& buff = *it;

		buff.Tick();

		if (buff.ShouldDie())
		{
			buff.myBlueprint.RemoveFrom(buff.myCharacter);
			it = buffComponent.Buffs.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void CharacterBuffsFunctionality::Inspect()
{
	SCharacterBuffComponent& buffComponent = Get<SCharacterBuffComponent>();
	
	ImGui::Separator();

	ImGui::Text("Buffs:");
	for(auto& buff : buffComponent.Buffs)
	{
		buff.Inspect();
		ImGui::Separator();
	}
	ImGui::Separator();
}
