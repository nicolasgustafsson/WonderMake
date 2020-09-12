#include "pch.h"
#include "CharacterEffect.h"
#include "Character/CharacterFunctionality.h"

void CharacterEffectDamage::Apply(CharacterFunctionality& aCharacter)
{
	aCharacter.Damage(static_cast<i32>(myDamageAmount));
}

void CharacterEffectDamage::Inspect()
{
	ImGui::Text("Damages the character for %f", myDamageAmount);
}

void CharacterEffectApplyBuff::Apply(CharacterFunctionality& aCharacter)
{
	aCharacter.Get<CharacterBuffsFunctionality>().ApplyBuff(aCharacter, myBuffToApply);
}

void CharacterEffectApplyBuff::Inspect()
{
	ImGui::Text("Applies following buff: ");
	if (ImGui::TreeNode("Buff"))
	{
		myBuffToApply.Inspect();
		ImGui::TreePop();
	}
}

void CharacterEffectHeal::Apply(CharacterFunctionality& aCharacter)
{
	aCharacter.Heal(static_cast<i32>(myHealAmount));
}

void CharacterEffectHeal::Inspect()
{
	ImGui::Text("Heals the character for %f", myHealAmount);
}
