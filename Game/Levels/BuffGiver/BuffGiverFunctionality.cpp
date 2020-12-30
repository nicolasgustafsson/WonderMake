#include "pch.h"
#include "BuffGiverFunctionality.h"
#include "Character/CharacterFunctionality.h"

REGISTER_COMPONENT(SBuffGiverComponent);
REGISTER_FUNCTIONALITY(BuffGiverFunctionality);

void BuffGiverFunctionality::Initialize(BuffBlueprint& aBuffBlueprint, const f32 aRadius)
{
	SBuffGiverComponent& buffGiverComponent = Get<SBuffGiverComponent>();

	buffGiverComponent.BuffBlueprint = &aBuffBlueprint;

	buffGiverComponent.Collider = &Get<CollisionFunctionality>().AddSphereCollider(*this, SVector2f::Zero(), aRadius);

	Get<CollisionFunctionality>().AddReaction<CharacterFunctionality>(*buffGiverComponent.Collider, [&aBuffBlueprint](CharacterFunctionality& aCharacter, Colliders::SCollisionInfo)
		{
			CharacterBuffsFunctionality& buffFunctionality = aCharacter.Get<CharacterBuffsFunctionality>();

			if (aCharacter.IsDead())
				return;

			if (!buffFunctionality.HasBuff(aBuffBlueprint))
			{
				buffFunctionality.ApplyBuff(aCharacter, aBuffBlueprint);
				WmLog("Gave the buff!");
			}
		});

	auto& renderObject = Get<SBuffGiverComponent>().RenderObject;
	renderObject.emplace();
	renderObject->SetAttribute<EVertexAttribute::Scale>(0, { aRadius + 10.f, aRadius + 10.f });
	renderObject->SetAttribute<EVertexAttribute::Color>(0, aBuffBlueprint.myBuffType == EBuffType::Buff ? SColor::Green() : SColor::Red());
}

void BuffGiverFunctionality::Tick()
{
	auto& renderObject = Get<SBuffGiverComponent>().RenderObject;

	renderObject->SetAttribute<EVertexAttribute::Position>(0, Get<TransformFunctionality2D>().GetPosition());
	
	renderObject->Render();
}
