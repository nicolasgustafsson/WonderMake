#include "pch.h"
#include "MovesetOperations.h"
#include "Movesets/Moveset.h"
#include "../../Attribute/Attribute.h"
#include "../../Sketch/Sketch.h"
#include "../SwingDesigner/SwingDesigner.h"
#include "../SwingDesigner/SwingAttributes/SwingAttributes.h"
#include "../MovesetAttributes.h"
#include <Randomizer/Randomizer.h>

bool GenerateMoveTypes::IsEligible(const Sketch& aSketch) const
{
	return aSketch.GetAttribute<SAmountOfMoves>()
		&& aSketch.ContainsAttribute<SMovesetType>();
}

void GenerateMoveTypes::Perform(Sketch& aSketch) const
{
	SMoveTypes moveTypes;
	const u32 size = aSketch.GetAttribute<SAmountOfMoves>()->TotalMoves;

	moveTypes.Types.reserve(static_cast<size_t>(size));

	for (u32 i = 0; i < size; i++)
	{
		switch (aSketch.GetAttribute<SMovesetType>()->Type)
		{
		case EMovesetType::Melee:
			moveTypes.Types.emplace_back(SystemPtr<Randomizer>()->SelectOne<ESwingType, ESwingType::Slash, ESwingType::Stab>());
			break;
		case EMovesetType::Ranged:
			moveTypes.Types.emplace_back(ESwingType::Projectile);
			break;
		}
	}

	aSketch.AddAttribute(std::move(moveTypes));
}

bool RandomizeMoveTypes::IsEligible(const Sketch& aSketch) const
{
	return aSketch.GetAttribute<SAmountOfMoves>()
		&& !aSketch.ContainsAttribute<SMoveTypes>();
}

void RandomizeMoveTypes::Perform(Sketch& aSketch) const
{
	SMoveTypes moveTypes;
	const u32 size = aSketch.GetAttribute<SAmountOfMoves>()->TotalMoves;

	moveTypes.Types.reserve(static_cast<size_t>(size));

	for (u32 i = 0; i < size; i++)
	{
		moveTypes.Types.emplace_back(SystemPtr<Randomizer>()->SelectOne<ESwingType, ESwingType::Slash, ESwingType::Stab, ESwingType::Projectile>());
	}

	aSketch.AddAttribute(std::move(moveTypes));
}

bool AddMoves::IsEligible(const Sketch& aSketch) const
{
	return aSketch.ContainsAttribute<SMoveTypes>();
}

void AddMoves::Perform(Sketch& aSketch) const
{
	const EMeleeMovesetType movesetType = aSketch.GetAttribute<SMeleeMovesetTypeAttribute>()->Moveset;

	auto&& moveTypes = aSketch.GetAttribute<SMoveTypes>()->Types;

	SGenericAttribute<SMoveset> genericSwingAttribute;
	for (size_t i = 0; i < moveTypes.size(); i++)
	{
		SwingDesigner swingDesigner;

		Sketch newSketch;
		SSwingTypeAttribute swingTypeAttribute;
		swingTypeAttribute.Type = moveTypes[i];
		SSwingSpeedAttribute swingSpeedAttribute;

		newSketch.AddAttribute(swingTypeAttribute);

		const bool isLastAttack = i == moveTypes.size() - 1;
		SSwingThreatAttribute swingThreatAttribute;

		switch (movesetType)
		{
		case EMeleeMovesetType::ManyWeakAttacks:
			swingThreatAttribute.SwingThreat = ESwingThreat::Pathetic;
			swingSpeedAttribute.SwingSpeed = ESwingSpeed::Cheetah;
			break;
		case EMeleeMovesetType::NormalCombo:
			swingThreatAttribute.SwingThreat = ESwingThreat::Normal;
			swingSpeedAttribute.SwingSpeed = ESwingSpeed::Snake;
			break;
		case EMeleeMovesetType::SlowDeadlySwings:
			swingThreatAttribute.SwingThreat = ESwingThreat::Stronk;
			swingSpeedAttribute.SwingSpeed = ESwingSpeed::Snail;
			break;
		case EMeleeMovesetType::WeakComboWithStrongFinisher:
			swingThreatAttribute.SwingThreat = isLastAttack ? ESwingThreat::Stronk : ESwingThreat::Pathetic;
			swingSpeedAttribute.SwingSpeed = isLastAttack ? ESwingSpeed::Snake : ESwingSpeed::Cheetah;
			break;
		}

		newSketch.AddAttribute(std::move(swingThreatAttribute));
		newSketch.AddAttribute(std::move(swingSpeedAttribute));

		const SAttackMove swing = swingDesigner.Design(newSketch); //why are we sending over the same sketch, should we not make a new one for the swing?

		genericSwingAttribute.Attribute.Moves.push_back(swing);
	}

	aSketch.AddAttribute(std::move(genericSwingAttribute));
}

bool SmashTogetherMoveset::IsEligible(const Sketch& aSketch) const
{
	return aSketch.ContainsAttribute<SGenericAttribute<SMoveset>>();
}

void SmashTogetherMoveset::Perform(Sketch& aSketch) const
{
	auto moveset = aSketch.GetAttribute<SGenericAttribute<SMoveset>>()->Attribute;

	SDesignedObjectAttribute<SMoveset> attribute;
	attribute.FinishedDesign = moveset;


	aSketch.AddAttribute<SDesignedObjectAttribute<SMoveset>>(std::move(attribute));
}

bool DetermineAmountOfMoves::IsEligible(const Sketch& /*aSketch*/) const
{
	return true;
}

void DetermineAmountOfMoves::Perform(Sketch& aSketch) const
{
	const EMeleeMovesetType movesetType = aSketch.GetAttribute<SMeleeMovesetTypeAttribute>()->Moveset;

	SAmountOfMoves attribute;

	switch (movesetType)
	{
	case EMeleeMovesetType::ManyWeakAttacks:
		attribute.TotalMoves = SystemPtr<Randomizer>()->GetRandomNumber<u32>(1, 7);
		break;
	case EMeleeMovesetType::NormalCombo:
		attribute.TotalMoves = SystemPtr<Randomizer>()->GetRandomNumber<u32>(2, 4);
		break;
	case EMeleeMovesetType::WeakComboWithStrongFinisher:
		attribute.TotalMoves = SystemPtr<Randomizer>()->GetRandomNumber<u32>(3, 5);
		break;
	case EMeleeMovesetType::SlowDeadlySwings:
		attribute.TotalMoves = SystemPtr<Randomizer>()->GetRandomNumber<u32>(1, 3);
		break;
	default:
		break;
	}

	aSketch.AddAttribute(std::move(attribute));
}

bool RandomizeMovesetType::IsEligible(const Sketch& aSketch) const
{
	return !aSketch.ContainsAttribute<SMeleeMovesetTypeAttribute>();
}

void RandomizeMovesetType::Perform(Sketch& aSketch) const
{
	SMeleeMovesetTypeAttribute meleeMoveset;

	meleeMoveset.Moveset = SystemPtr<Randomizer>()->SelectOne<EMeleeMovesetType, EMeleeMovesetType::ManyWeakAttacks, EMeleeMovesetType::NormalCombo, EMeleeMovesetType::SlowDeadlySwings, EMeleeMovesetType::WeakComboWithStrongFinisher>();
	aSketch.AddAttribute(std::move(meleeMoveset));
}
