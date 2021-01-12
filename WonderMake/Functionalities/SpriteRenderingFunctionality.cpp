#include "pch.h"
#include "SpriteRenderingFunctionality.h"
#include <iostream>
#include "Resources/AssetDatabase.h"
#include "Graphics/Texture.h"

REGISTER_FUNCTIONALITY(SpriteRenderingFunctionality);

void SpriteRenderingFunctionality::Tick()
{
	auto& spriteComponent = Get<SSpriteComponent>();
	if (!spriteComponent.RenderObject || spriteComponent.IsHidden)
		return;

	spriteComponent.RenderObject->SetAttribute<EVertexAttribute::Position>(0, Get<STransformComponent>().Position);
	spriteComponent.RenderObject->SetAttribute<EVertexAttribute::Rotation>(0, Get<STransformComponent>().Rotation);
	spriteComponent.RenderObject->Render();
}
void SpriteRenderingFunctionality::SetTexture(const std::string_view aAssetLink)
{
	if (Get<SSpriteComponent>().RenderObject)
		Get<SSpriteComponent>().RenderObject->SetTexture(SystemPtr<AssetDatabase<Texture>>()->GetResource(aAssetLink));
	else
		Get<SSpriteComponent>().RenderObject.emplace(aAssetLink);
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

void SpriteRenderingFunctionality::SetColor(const SColor aColor)
{
	Get<SSpriteComponent>().RenderObject->SetColor(aColor);
}

void SpriteRenderingFunctionality::Hide() noexcept
{
	Get<SSpriteComponent>().IsHidden = true;
}

void SpriteRenderingFunctionality::Show() noexcept
{
	Get<SSpriteComponent>().IsHidden = false;
}
