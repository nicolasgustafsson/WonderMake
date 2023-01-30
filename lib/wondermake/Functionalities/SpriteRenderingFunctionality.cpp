#include "SpriteRenderingFunctionality.h"

#include "Functionalities/TransformFunctionality.h"

#include "Graphics/Texture.h"

#include "Resources/AssetDatabase.h"
#include "Resources/ResourceSystem.h"

#include "wondermake-engine/ConfigurationEngine.h"

#include "wondermake-base/ConfigurationSystem.h"

#include <iostream>

WM_REGISTER_FUNCTIONALITY(SpriteRenderingFunctionality);

void SpriteRenderingFunctionality::Initialize(SInitializationInfo aInfo)
{
	SetTexture(std::move(aInfo.FilePath));

	SetScale(aInfo.Scale);
	SetColor(aInfo.Color);
	SetOrigin(aInfo.Origin);
	SetRenderLayer(aInfo.RenderLayer);
	SetRenderOrder(aInfo.RenderOrder);
}

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

void SpriteRenderingFunctionality::SetTexture(std::string_view aAssetLink)
{
	auto& textureDatabase = Get<AssetDatabase<Texture>>();

	SetTexture(textureDatabase.GetResource(aAssetLink));
}

void SpriteRenderingFunctionality::SetTexture(const FilePath& aFilePath)
{
	auto& textureResourceSys = Get<ResourceSystem<Texture>>();

	SetTexture(textureResourceSys.GetResource(aFilePath));
}

[[nodiscard]] SVector2u SpriteRenderingFunctionality::GetTextureSize() const noexcept
{
	auto& spriteComponent = Get<SSpriteComponent>();

	if (!spriteComponent.RenderObject)
		return SVector2u::Zero();

	auto texture = spriteComponent.RenderObject->GetTexture(0);

	if (!texture)
		return SVector2u::Zero();

	return SVector2u(
		static_cast<u32>(texture->GetWidth()),
		static_cast<u32>(texture->GetHeight()));
}

void SpriteRenderingFunctionality::SetScale(const SVector2f aScale)
{
	Get<SSpriteComponent>().RenderObject->SetAttribute<EVertexAttribute::Scale>(0, aScale);
}

void SpriteRenderingFunctionality::SetOrigin(const SVector2f aOrigin)
{
	Get<SSpriteComponent>().RenderObject->SetAttribute<EVertexAttribute::Origin>(0, aOrigin);
}

void SpriteRenderingFunctionality::SetColor(const SColor aColor)
{
	Get<SSpriteComponent>().RenderObject->SetColor(aColor);
}

void SpriteRenderingFunctionality::SetRenderLayer(std::string_view aRenderLayer)
{
	Get<SSpriteComponent>().RenderObject->SetRenderLayer(std::move(aRenderLayer));
}

void SpriteRenderingFunctionality::SetRenderOrder(i32 aRenderOrder)
{
	Get<SSpriteComponent>().RenderObject->SetRenderOrder(aRenderOrder);
}

void SpriteRenderingFunctionality::Hide() noexcept
{
	Get<SSpriteComponent>().IsHidden = true;
}

void SpriteRenderingFunctionality::Show() noexcept
{
	Get<SSpriteComponent>().IsHidden = false;
}

void SpriteRenderingFunctionality::SetTexture(ResourceProxy<Texture>&& aTexture)
{
	auto& spriteComponent = Get<SSpriteComponent>();

	auto texture = aTexture && aTexture->IsValid()
		? std::move(aTexture)
		: Get<ResourceSystem<Texture>>().GetResource(Get<ConfigurationSystem>().Get<FilePath>(ConfigurationEngine::MissingTexturePath, FilePath()));

	if (spriteComponent.RenderObject)
		spriteComponent.RenderObject->SetTexture(std::move(texture));
	else
		spriteComponent.RenderObject.emplace(
			Get<ResourceSystem<Shader<EShaderType::Vertex>>>(),
			Get<ResourceSystem<Shader<EShaderType::Fragment>>>(),
			Get<ResourceSystem<Shader<EShaderType::Geometry>>>(),
			std::move(texture));
}
