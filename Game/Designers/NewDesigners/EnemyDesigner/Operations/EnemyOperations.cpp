#include "pch.h"

#include "EnemyOperations.h"

#include "../../Attribute/Attribute.h"
#include "../../Sketch/Sketch.h"

#include "../../MovesetDesigner/MovesetDesigner.h"
#include "../../ColorDesigner/ColorDesigner.h"

#include "Enemy/EnemyControllerFunctionality.h"

#include "Object/Object.h"

#include "Utility/Palette.h"

bool CreateEnemyOperation::IsEligible(const Sketch& aSketch) const
{
	return aSketch.ContainsAttribute<SGenericAttribute<SMoveset>>();
}

void CreateEnemyOperation::Perform(Sketch& aSketch) const
{
	SDesignedObjectAttribute<Object> enemy;

	auto&& movesetAttribute = aSketch.GetAttribute<SGenericAttribute<SMoveset>>();
	auto&& enemyController = SystemPtr<FunctionalitySystemDelegate<EnemyControllerFunctionality>>()->AddFunctionality(enemy.FinishedDesign);
	auto&& colorAttribute = aSketch.GetAttribute<SGenericAttribute<SColor>>();
	auto&& sprite = SystemPtr<FunctionalitySystemDelegate<SpriteRenderingFunctionality>>()->AddFunctionality(enemy.FinishedDesign);

	enemyController.SetMoveset(std::move(movesetAttribute->Attribute));
	sprite.SetColor(std::move(colorAttribute->Attribute));

	aSketch.AddAttribute(SDesignedObjectAttribute<Object>(std::move(enemy)));
}

bool GenerateEnemyMovesetOperation::IsEligible(const Sketch& /*aSketch*/) const
{
	return true;
}

void GenerateEnemyMovesetOperation::Perform(Sketch& aSketch) const
{
	MovesetDesigner movesetDesigner;
	Sketch newSketch;
	SGenericAttribute<SMoveset> movesetAttribute;

	auto moveset = movesetDesigner.Design(newSketch);

	movesetAttribute.Attribute = std::move(moveset);

	aSketch.AddAttribute(std::move(movesetAttribute));
}

bool GenerateEnemyColorOperation::IsEligible(const Sketch& /*aSketch*/) const
{
	return true;
}

void GenerateEnemyColorOperation::Perform(Sketch& aSketch) const
{
	ColorDesigner colorDesigner;
	Sketch newSketch = aSketch;
	SGenericAttribute<SColor> colorAttribute;

	auto moveset = colorDesigner.Design(newSketch);

	colorAttribute.Attribute = std::move(moveset);

	aSketch.AddAttribute(std::move(colorAttribute));
}
