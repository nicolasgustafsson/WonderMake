#include "pch.h"
#include "SpriteRenderingFunctionality.h"
#include "TransformFunctionality.h"
#include <iostream>
#include "Resources/AssetDatabase.h"
#include "Graphics/Texture.h"

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
void SpriteRenderingFunctionality::SetTexture(const std::string_view aAssetLink)
{
	SSpriteComponent& spriteComponent = Get<SSpriteComponent>();
	if (spriteComponent.RenderObject)
		spriteComponent.RenderObject->SetTexture(SystemPtr<AssetDatabase<Texture>>()->GetResource(aAssetLink));
	else
		spriteComponent.RenderObject.emplace(aAssetLink);
}

void SpriteRenderingFunctionality::SetFragmentShader(const std::filesystem::path& aAssetPath)
{
	auto& spriteComponent = Get<SSpriteComponent>();
	if (!spriteComponent.RenderObject)
	{
		WmLog(TagWarning, "Tried to set fragment shader on a sprite render object that doesn't exist!");
		return;
	}

	spriteComponent.RenderObject->SetShader(EShaderType::Fragment, aAssetPath);
	//spriteComponent.RenderObject->SetProperty("TextureSize", )
}

void SpriteRenderingFunctionality::SetScale(const SVector2f aScale)
{
	Get<SSpriteComponent>().RenderObject->SetScale(aScale);
}

void SpriteRenderingFunctionality::SetScale(const f32 aScale)
{
	Get<SSpriteComponent>().RenderObject->SetScale(aScale);
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

void SpriteRenderingFunctionality::SetRenderOrder(const i32 aOrder)
{
	Get<SSpriteComponent>().RenderObject->SetRenderOrder(aOrder);
}

void SpriteRenderingFunctionality::Hide() noexcept
{
	Get<SSpriteComponent>().IsHidden = true;
}

void SpriteRenderingFunctionality::Show() noexcept
{
	Get<SSpriteComponent>().IsHidden = false;
}
