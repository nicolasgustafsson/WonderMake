#include "stdafx.h"
#include "SpriteRenderingFunctionality.h"


SpriteRenderingFunctionality::SpriteRenderingFunctionality(Object* aOwner)
	:Super(aOwner)
{
}


SpriteRenderingFunctionality::~SpriteRenderingFunctionality()
{
}

void SpriteRenderingFunctionality::Tick()
{
	if (!Get<SSpriteComponent>().RenderObject)
		return;

	Get<SSpriteComponent>().RenderObject->SetAttribute<EVertexAttribute::Position>(0, Get<STransformComponent>().Position);
	Get<SSpriteComponent>().RenderObject->Render();
}

void SpriteRenderingFunctionality::SetTexture(const std::filesystem::path& aTexturePath)
{
	Get<SSpriteComponent>().RenderObject.emplace(aTexturePath);
}

void SpriteRenderingFunctionality::SetScale(const SVector2f aScale)
{
	Get<SSpriteComponent>().RenderObject->SetAttribute<EVertexAttribute::Scale>(0, aScale);
}
