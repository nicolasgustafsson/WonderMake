#include "pch.h"

#include "EnemyOperations.h"

#include "../../Attribute/Attribute.h"
#include "../../Sketch/Sketch.h"

#include "../../MovesetDesigner/MovesetDesigner.h"

#include "Enemy/EnemyControllerFunctionality.h"

#include "Object/Object.h"

bool CreateEnemyOperation::IsEligible(const Sketch& aSketch) const
{
	return aSketch.ContainsAttribute<SGenericAttribute<SMoveset>>();
}

void CreateEnemyOperation::Perform(Sketch& aSketch) const
{
	SDesignedObjectAttribute<Object> enemy;

	auto&& movesetAttribute = aSketch.GetAttribute<SGenericAttribute<SMoveset>>();

	auto&& enemyController = SystemPtr<FunctionalitySystemDelegate<EnemyControllerFunctionality>>()->AddFunctionality(enemy.FinishedDesign);

	enemyController.SetMoveset(std::move(movesetAttribute->Attribute));

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
