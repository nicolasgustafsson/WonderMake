#include "pch.h"
#include "SpriteRenderingFunctionality.h"
#include "TransformFunctionality.h"
#include <iostream>
#include "Resources/AssetDatabase.h"
#include "Graphics/Texture.h"
#include "Assets/AssetManager.h"

REGISTER_FUNCTIONALITY(SpriteRenderingFunctionality);

void SpriteRenderingFunctionality::Tick()
{
	auto& spriteComponent = Get<SSpriteComponent>();
	if (!spriteComponent.RenderObject || spriteComponent.IsHidden)
		return;

	const auto& transform = Get<TransformFunctionality2D>();

	spriteComponent.RenderObject->SetAttribute<EVertexAttribute::Position>(0, transform.GetPosition());
	spriteComponent.RenderObject->SetAttribute<EVertexAttribute::Rotation>(0, transform.GetRotation());
	spriteComponent.RenderObject->Render();
}

//void SpriteRenderingFunctionality::SetTexture(const std::string_view aAssetLink)
//{
//	SSpriteComponent& spriteComponent = Get<SSpriteComponent>();
//	if (spriteComponent.RenderObject)
//		spriteComponent.RenderObject->SetTexture(SystemPtr<AssetDatabase<Texture>>()->GetResource(aAssetLink));
//	else
//		spriteComponent.RenderObject.emplace(aAssetLink);
//}

void SpriteRenderingFunctionality::SetTexture(const std::filesystem::path& aAssetPath)
{
	SSpriteComponent& spriteComponent = Get<SSpriteComponent>();

	std::optional<AssetRef<Texture>> texture = SystemPtr<AssetManager<Texture>>()->GetAsset(aAssetPath);

	if (!texture)
		return;

	if (spriteComponent.RenderObject)
		spriteComponent.RenderObject->SetTexture(*texture);
	else
		spriteComponent.RenderObject.emplace(*texture);
}

void SpriteRenderingFunctionality::SetScale(const SVector2f aScale)
{
	SSpriteComponent& spriteComponent = Get<SSpriteComponent>();
	if (spriteComponent.RenderObject)
		spriteComponent.RenderObject->SetAttribute<EVertexAttribute::Scale>(0, aScale);
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
