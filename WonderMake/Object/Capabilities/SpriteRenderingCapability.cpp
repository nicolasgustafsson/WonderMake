#include "stdafx.h"
#include "SpriteRenderingCapability.h"


SpriteRenderingCapability::SpriteRenderingCapability(Object* aOwner)
	:mySpriteComponent(aOwner)
	,myTransformComponent(aOwner)
{
}


SpriteRenderingCapability::~SpriteRenderingCapability()
{
}

void SpriteRenderingCapability::Tick()
{
	if (!mySpriteComponent->RenderObject)
		return;

	mySpriteComponent->RenderObject->SetAttribute<EVertexAttribute::Position>(0, myTransformComponent->Position);
	mySpriteComponent->RenderObject->Render();
}

void SpriteRenderingCapability::SetTexture(const std::filesystem::path& aTexturePath)
{
	mySpriteComponent->RenderObject.emplace(aTexturePath);
}

void SpriteRenderingCapability::SetScale(const SVector2f aScale)
{
	mySpriteComponent->RenderObject->SetAttribute<EVertexAttribute::Scale>(0, aScale);
}
