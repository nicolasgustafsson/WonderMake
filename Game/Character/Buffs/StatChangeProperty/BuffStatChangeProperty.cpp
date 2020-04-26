#include "pch.h"
#include "BuffStatChangeProperty.h"
#include "Character/Stats/CharacterStatsFunctionality.h"
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