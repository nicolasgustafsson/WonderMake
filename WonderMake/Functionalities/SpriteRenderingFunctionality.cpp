#include "pch.h"
#include "SpriteRenderingFunctionality.h"
#include <iostream>


SpriteRenderingFunctionality::SpriteRenderingFunctionality(Object& aOwner)
	:Super(aOwner)
{
}

void SpriteRenderingFunctionality::Tick()
{
	auto& SpriteComponent = Get<SSpriteComponent>();
	if (!SpriteComponent.RenderObject)
		return;

	const float currentTime = myTimeKeeperPtr->GetGameTime() * 2;
	const SVector2f position = Get<STransformComponent>().Position;

	SpriteComponent.RenderObject->SetAttribute<EVertexAttribute::Position>(0, position);
	SpriteComponent.RenderObject->Render();

	SVector2f endPosition = position;
	endPosition += SVector2f{ std::cosf(currentTime) * 166.0f, std::sinf(currentTime) * 166.0f };

	endPosition = endPosition * std::sinf(currentTime / 2.0f);
}

void SpriteRenderingFunctionality::SetTexture(const std::filesystem::path& aTexturePath)
{
	Get<SSpriteComponent>().RenderObject.emplace(aTexturePath);
}

void SpriteRenderingFunctionality::SetScale(const SVector2f aScale)
{
	Get<SSpriteComponent>().RenderObject->SetAttribute<EVertexAttribute::Scale>(0, aScale);
}
