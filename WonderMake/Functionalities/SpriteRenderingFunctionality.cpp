#include "pch.h"
#include "SpriteRenderingFunctionality.h"
#include <iostream>


SpriteRenderingFunctionality::SpriteRenderingFunctionality(Object& aOwner)
	: Super(aOwner)
{}

void SpriteRenderingFunctionality::Tick()
{
	auto& spriteComponent = Get<SSpriteComponent>();
	if (!spriteComponent.RenderObject || spriteComponent.bIsHidden)
		return;

	const SVector2f position = Get<STransformComponent>().Position;
	spriteComponent.RenderObject->SetAttribute<EVertexAttribute::Position>(0, position);
	spriteComponent.RenderObject->Render();
}

void SpriteRenderingFunctionality::SetTexture(const std::filesystem::path& aTexturePath)
{
	Get<SSpriteComponent>().RenderObject.emplace(aTexturePath);
}

void SpriteRenderingFunctionality::SetScale(const SVector2f aScale)
{
	Get<SSpriteComponent>().RenderObject->SetAttribute<EVertexAttribute::Scale>(0, aScale);
}

void SpriteRenderingFunctionality::SetRotation(const f32 aRotation)
{
	Get<SSpriteComponent>().RenderObject->SetAttribute<EVertexAttribute::Rotation>(0, aRotation);
}

void SpriteRenderingFunctionality::SetOrigin(const SVector2f aOrigin)
{
	Get<SSpriteComponent>().RenderObject->SetAttribute<EVertexAttribute::Origin>(0, aOrigin);
}

void SpriteRenderingFunctionality::Hide()
{
	Get<SSpriteComponent>().bIsHidden == true;
}

void SpriteRenderingFunctionality::Show()
{
	Get<SSpriteComponent>().bIsHidden == true;
}
