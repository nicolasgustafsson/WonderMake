#include "pch.h"

#include "MovesetFunctionality.h"

#include "Actions/ActionFunctionality.h"
#include "Character/CharacterFunctionality.h"
#include "Weapons/WeaponSwingHitShapeAction.h"

REGISTER_COMPONENT(SMovesetComponent);
REGISTER_FUNCTIONALITY(MovesetFunctionality);

void MovesetFunctionality::SetMoveset(const SMoveset& aMoveset)
{
	auto& movesetComp = Get<SMovesetComponent>();

	movesetComp.Moveset = aMoveset;
	movesetComp.CurrentSwingIndex = 0;
}

void MovesetFunctionality::Perform(SVector2f aDirection)
{
	auto& movesetComp = Get<SMovesetComponent>();
	auto& actionFunctionality = Get<ActionFunctionality>();

	auto&& moveset = movesetComp.Moveset;

	if (moveset.Moves.empty())
		return;

	if (!actionFunctionality.WasLastActionOfType<WeaponSwingHitShapeAction>() || actionFunctionality.TimeSinceLastAction() > 0.5f)
		movesetComp.CurrentSwingIndex = 0;

	movesetComp.CurrentSwingIndex %= moveset.Moves.size();

	EActionResult result = actionFunctionality.StartAction(WeaponSwingHitShapeAction
	(
		Get<CharacterFunctionality>()
		, aDirection
		, moveset.Moves[movesetComp.CurrentSwingIndex]
	));

	if (result == EActionResult::Succeeded)
		movesetComp.CurrentSwingIndex++;
}
