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
	for (auto it = buffs.begin(); it != buffs.end(); it++)
	{
		auto& buff = *it;
		buff.myBlueprint.RemoveFrom(buff.myCharacter);

		it = buffs.erase(it);
		if (it == buffs.end())
			return;
	}
}

bool CharacterBuffsFunctionality::HasBuff(BuffBlueprint& aBuffBlueprint) const
{
	auto& buffs = Get<SCharacterBuffComponent>().Buffs;

	for (auto& buff : buffs)
	{
		if (&buff.myBlueprint == &aBuffBlueprint)
			return true;
	}

	return false;
}

void CharacterBuffsFunctionality::Tick()
{
	const f32 deltaTime = SystemPtr<TimeKeeper>()->GetDeltaSeconds();
	SCharacterBuffComponent& buffComponent = Get<SCharacterBuffComponent>();

	for (auto it = buffComponent.Buffs.begin(); it != buffComponent.Buffs.end(); it++)
	{
		auto& buff = *it;

		buff.Tick();

		if (buff.ShouldDie())
		{
			buff.myBlueprint.RemoveFrom(buff.myCharacter);

			it = buffComponent.Buffs.erase(it);
			if (it == buffComponent.Buffs.end())
				return;
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
