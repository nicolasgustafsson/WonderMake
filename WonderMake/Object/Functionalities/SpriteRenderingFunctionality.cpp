#include "stdafx.h"
#include "SpriteRenderingFunctionality.h"


SpriteRenderingFunctionality::SpriteRenderingFunctionality(Object* aOwner)
	:mySpriteComponent(aOwner)
	,myTransformComponent(aOwner)
{
}


SpriteRenderingFunctionality::~SpriteRenderingFunctionality()
{
}

void SpriteRenderingFunctionality::Tick()
{
	if (!mySpriteComponent->RenderObject)
		return;

	mySpriteComponent->RenderObject->SetAttribute<EVertexAttribute::Position>(0, myTransformComponent->Position);
	mySpriteComponent->RenderObject->Render();
}

void SpriteRenderingFunctionality::SetTexture(const std::filesystem::path& aTexturePath)
{
	mySpriteComponent->RenderObject.emplace(aTexturePath);
}

void SpriteRenderingFunctionality::SetScale(const SVector2f aScale)
{
	mySpriteComponent->RenderObject->SetAttribute<EVertexAttribute::Scale>(0, aScale);
}
