#include "pch.h"

#include "ColorOperations.h"

#include "../Attribute/Attribute.h"
#include "../Sketch/Sketch.h"

#include "../MovesetDesigner/Moveset.h"

#include "Randomizer/Randomizer.h"

#include "Utilities/Math.h"

bool RandomColorOperation::IsEligible(const Sketch& aSketch) const
{
	return !aSketch.ContainsAttribute<SDesignedObjectAttribute<SColor>>();
}

void RandomColorOperation::Perform(Sketch& aSketch) const
{
	SDesignedObjectAttribute<SColor> color;

	color.FinishedDesign = SystemPtr<Randomizer>()->GetRandomNumber<u32>(0x000000FF, 0xFFFFFFFF);

	aSketch.AddAttribute(std::move(color));
}

bool MovesetToColorOperation::IsEligible(const Sketch& aSketch) const
{
	return aSketch.ContainsAttribute<SGenericAttribute<SMoveset>>();
}

void MovesetToColorOperation::Perform(Sketch& aSketch) const
{
	auto&& movesetAttibute = aSketch.GetAttribute<SGenericAttribute<SMoveset>>();

	auto&& moveset = movesetAttibute->Attribute;

	auto&& move = moveset.Swings.front();
	SDesignedObjectAttribute<SColor> color;

	constexpr f32 strengthMax = 3.f;
	constexpr f32 strengthMin = .25f;
	constexpr f32 speedMax = .2f * 2.f;
	constexpr f32 speedMin = .2f * .33f;

	color.FinishedDesign.R = (move.DamageMultipiler - strengthMin) / (strengthMax - strengthMin);
	color.FinishedDesign.G = 0;
	color.FinishedDesign.B = 1.f - ((move.SwingTime - speedMin) / (speedMax - speedMin));

	aSketch.AddAttribute(std::move(color));
}
