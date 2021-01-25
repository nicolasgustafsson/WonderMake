#include "pch.h"
#include "MovesetOperations.h"
#include "../Moveset.h"
#include "../../Attribute/Attribute.h"
#include "../../Sketch/Sketch.h"
#include "../SwingDesigner/SwingDesigner.h"
#include "../SwingDesigner/SwingAttributes/SwingAttributes.h"
#include "../MovesetAttributes.h"
#include <Randomizer/Randomizer.h>

bool AddMoves::IsEligible(const Sketch& aSketch) const
{
	return aSketch.ContainsAttribute<SAmountOfMoves>();
}

void AddMoves::Perform(Sketch& aSketch) const
{
	SGenericAttribute<SMoveset> genericSwingAttribute;
	for (i32 i = 0; i < aSketch.GetAttribute<SAmountOfMoves>()->MovesLeft; i++)
	{
		SwingDesigner swingDesigner;

		Sketch newSketch;
		SSwingTypeAttribute swingTypeAttribute;
		swingTypeAttribute.Type = static_cast<ESwingType>(SystemPtr<Randomizer>()->GetRandomNumber(0, 1));

		newSketch.AddAttribute<SSwingTypeAttribute>(swingTypeAttribute);
		const SSwing swing = swingDesigner.Design(newSketch); //why are we sending over the same sketch, should we not make a new one for the swing?

		genericSwingAttribute.Attribute.Swings.push_back(swing);
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

bool DetermineAmountOfMoves::IsEligible(const Sketch& aSketch) const
{
	return true;
}

void DetermineAmountOfMoves::Perform(Sketch& aSketch) const
{
	SAmountOfMoves attribute;
	attribute.MovesLeft = SystemPtr<Randomizer>()->GetRandomNumber<u32>(4, 5);

	aSketch.AddAttribute(std::move(attribute));

}
