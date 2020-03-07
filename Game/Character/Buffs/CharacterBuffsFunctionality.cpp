#include "pch.h"
#include "CharacterBuffsFunctionality.h"

CharacterBuffsFunctionality::CharacterBuffsFunctionality(Object& aOwner)
	: Super(aOwner)
{

}

void CharacterBuffsFunctionality::ApplyBuff(class CharacterFunctionality& aCharacter, BuffBlueprint& aBlueprint)
{
	SCharacterBuffComponent& buffComponent = Get<SCharacterBuffComponent>();

	Buff buffInstance = aBlueprint.Instantiate(aCharacter);

	buffComponent.Buffs.insert(std::move(buffInstance));
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
		buff.myTimeLeft -= deltaTime;

		if (buff.myTimeLeft < 0.f)
		{
			buff.myBlueprint.RemoveFrom(buff.myCharacter);

			it = buffComponent.Buffs.erase(it);
			if (it == buffComponent.Buffs.end())
				return;
		}
	}
}
